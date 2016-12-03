/* scull.c */
#include <asm-generic/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>
#include <uapi/asm-generic/errno-base.h>
#include "scull.h"

MODULE_LICENSE("Dual BSD/GPL");

static void scull_free_qset_list(struct scull_qset *head)
{
        struct scull_qset *tmp;

        if (!head->next)
                return;

        tmp = head->next->next;
        kfree(head->next);
        head->next = head->next->next;

        scull_free_qset_list(head);

        /* finanl return */
        return;
}

loff_t scull_llseek(struct file *file, loff_t off, int num)
{
        return 0;
}

ssize_t scull_read(struct file *file, char __user *buf, size_t num, loff_t *off)
{
        int i;
        int qset_n;
        int quantum_n;
        int count;
        int qset_bytes;
        int read_bytes;
        int off_n = *off;
        struct scull_dev *scull_dev = file->private_data;
        struct scull_qset *iter_qset = &scull_dev->head;

        /* find related quantum index */
        qset_bytes = scull_dev->qset * scull_dev->quantum;
        qset_n = off_n / qset_bytes;
        quantum_n = off_n % qset_bytes / quantum;
        count = off_n % qset_bytes % quantum;

        /* caculate how many bytes can be read out from this quantum */
        read_bytes = scull_dev->quantum - count;

        /* find the start point or no data there */
        for (i = 0; i <= qset_n; i++) {
                if (!iter_qset->next && i != qset_n)
                        return -ENOMEM;
                if (i != qset_n)
                        iter_qset = iter_qset->next;
        }

        /* no date */
        if (!iter_qset->data || !iter_qset->data[quantum_n])
                return -ENOMEM;

        /* copy data to userspace buffer */
        copy_to_user(buf, iter_qset->data[quantum_n] + count, read_bytes);

        /* update off */
        *off = off_n + read_bytes;

        /* error handle */

        /* test print */
        printk(KERN_ALERT "scull: read\n");

        return 0;
}

ssize_t scull_write(struct file *file, const char __user *buf, size_t num, loff_t *off)
{
        int i;
        int qset_n;
        int quantum_n;
        int count;
        int qset_bytes;
        int write_bytes;
        int off_n = *off;
        struct scull_dev *scull_dev = file->private_data;
        struct scull_qset *p_qset, *iter_qset = &scull_dev->head;

        /* find related quantum index */
        qset_bytes = scull_dev->qset * scull_dev->quantum;
        qset_n = off_n / qset_bytes;
        quantum_n = off_n % qset_bytes / quantum;
        count = off_n % qset_bytes % quantum;

        /* caculate how many bytes should be written into this quantum */
        write_bytes = scull_dev->quantum - count;

        /*
         * create related quantum. we will release quantum in scull_release
         * 1. find scull_qset from list; 2. create qset; 3. create quantum.
         */
        for (i = 0; i <= qset_n; i++) {
                if (iter_qset->next == NULL) {
                        /* create struct scull_qset; */
                        p_qset = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
                        if (p_qset->next == NULL)
                                return -ENOMEM;
                        memset(p_qset, 0, sizeof(struct scull_qset));

                        iter_qset->next = p_qset;
                        p_qset->next = NULL;
                }
                if (i != qset_n)
                        iter_qset = iter_qset->next;
        }

        /* iter_qset->data = create qset */
        if (!iter_qset->data) {
                iter_qset->data = kmalloc(sizeof(int *) * scull_dev->qset, GFP_KERNEL);
                if (iter_qset->data == NULL)
                        return -ENOMEM;
                memset(iter_qset->data, 0, sizeof(int *) * scull_dev->qset);
        }

        /* data[quantum] = create quantum */
        if (!iter_qset->data[quantum_n]) {
                iter_qset->data[quantum_n] = kmalloc(sizeof(char) * scull_dev->quantum, GFP_KERNEL);
                if (iter_qset->data[quantum_n] == NULL)
                        return -ENOMEM;
                memset(iter_qset->data[quantum_n], 0, sizeof(char) * scull_dev->quantum);
        }

        /* copy data from userspace buffer */
        copy_from_user(iter_qset->data[quantum_n] + count, buf, write_bytes);

        /* update off */
        *off = off_n + write_bytes;

        /* error handle */

        printk(KERN_ALERT "scull: write\n");

        /* return number of bytes we wrote */
        return write_bytes;
}

int scull_open(struct inode *inode, struct file *file)
{
        struct cdev *cdev;
        struct scull_dev *scull_dev;

        /* use inode to get cdev */
        cdev = inode->i_cdev;

        /* use cdev to get scull_dev */
        scull_dev = container_of(cdev, struct scull_dev, cdev);

        /* add scull_dev to file's private data, so .read/.write... can use  */
        file->private_data = scull_dev;

        return 0;
}

int scull_release(struct inode *inode, struct file *file)
{
#if 0
        int i;
        struct scull_dev *scull_dev = file->private_data;
        int qset = scull_dev->qset;
        struct scull_qset *iter_qset = &scull_dev->head;

        while (iter_qset != NULL) {
                if (iter_qset == &scull_dev->head) {
                        iter_qset = iter_qset->next;
                        continue;
                }

                if (!iter_qset->data) {
                        iter_qset = iter_qset->next;
                        continue;
                } else {
                        for (i = 0; i < qset; i++) {
                                if (!iter_qset->data[i]) {
                                        continue;
                                } else {
                                        kfree(iter_qset->data[i]);
                                }
                        }

                        kfree(iter_qset->data);
                }

                iter_qset = iter_qset->next;
        }

        /* free scull_qset list */
        iter_qset = &scull_dev->head;
        scull_free_qset_list(iter_qset);

        /* free scull_dev */
        kfree(scull_dev);
#endif
        return 0;
}

struct file_operations scull_fops = {
        .owner = THIS_MODULE,
        .llseek = scull_llseek,
        .read = scull_read,
        .write = scull_write,
        .open = scull_open,
        .release = scull_release,
};

static int __init scull_init(void)
{
        dev_t dev_id;
        int err = 0;
        struct scull_dev *scull_dev;
        unsigned int firstminor = 0;
        unsigned int count = 1;
        char *dev_name = "scull";

        /* create scull_dev */
        scull_dev = kmalloc(sizeof(struct scull_dev), GFP_KERNEL);
        if (!scull_dev) {
                
        }

        /* will offer different ways to set qset and quantum */

        scull_dev->head.data = NULL;
        scull_dev->head.next = NULL;
        scull_dev->qset = qset;
        scull_dev->quantum = quantum;
        scull_dev->size = 0;

        /* alloc dev_id */
        err = alloc_chrdev_region(&dev_id, firstminor, count, dev_name);

        /* register cdev */
        cdev_init(&scull_dev->cdev, &scull_fops);
        cdev_add(&scull_dev->cdev, dev_id, count);

        /* init scull_dev */

        return 0;
}

static void __exit scull_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(scull_init);
module_exit(scull_exit);

MODULE_AUTHOR("Sherlock");
MODULE_DESCRIPTION("The driver for scull Module");
MODULE_ALIAS("The driver for scull Module");
