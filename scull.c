/* scull.c */
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>
#include "scull.h"
#include "debug.h"

MODULE_LICENSE("Dual BSD/GPL");
module_param(qset, int, S_IRUGO);
module_param(quantum, int, S_IRUGO);

struct scull_dev *scull_device;
struct class *scull_class;

static void scull_free_qset_list(struct scull_qset *head)
{
        struct scull_qset *tmp;

        if (!head->next)
                return;

        tmp = head->next->next;
        kfree(head->next);
        head->next = tmp;

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
        int result;
        int qset_n;
        int quantum_n;
        int count;
        int qset_bytes;
        int read_bytes;
        int off_n = *off;
        struct scull_dev *scull_dev = file->private_data;
        struct scull_qset *iter_qset = &scull_dev->head;

        /* test scull_ioctl */
        pr_err("test_scull: scull_dev->qset: %d\n", scull_dev->qset);
        pr_err("test_scull: scull_dev->quantum: %d\n", scull_dev->quantum);

        /* very important!! */
        if (off_n > scull_dev->size - 1)
                return 0;
        if (off_n + num > scull_dev->size - 1)
                num = scull_dev->size - off_n;

        /* find related quantum index */
        qset_bytes = scull_dev->qset * scull_dev->quantum;
        qset_n = off_n / qset_bytes;
        quantum_n = off_n % qset_bytes / quantum;
        count = off_n % qset_bytes % quantum;

        /* caculate how many bytes can be read out from this quantum */
        if (num < scull_dev->quantum - count)
                read_bytes = num;
        else
                read_bytes = scull_dev->quantum - count;

        /* find the start point or no data there */
        for (i = 0; i <= qset_n; i++) {
                if (!iter_qset->next) {
                        ASSERT(0);
                        return -ENOMEM;
                }
                if (i != qset_n)
                        iter_qset = iter_qset->next;
        }

        iter_qset = iter_qset->next;

        /* no date */
        if (!iter_qset->data || !iter_qset->data[quantum_n]) {
                ASSERT(0);
                return -ENOMEM;
        }

        /* copy data to userspace buffer */
        result = copy_to_user(buf, iter_qset->data[quantum_n], read_bytes);
        if (result < 0) {
                pr_err("scull: can not copy data to user space\n");
                return result;
        }

        /* update off */
        *off = off_n + read_bytes;

        /* error handle */

        /* test print */
        printk(KERN_ALERT "scull: read\n");

        return read_bytes;
}

ssize_t scull_write(struct file *file, const char __user *buf, size_t num, loff_t *off)
{
        int i;
        int result;
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
        if (num < scull_dev->quantum - count)
                write_bytes = num;
        else
                write_bytes = scull_dev->quantum - count;

        mutex_lock(&scull_dev->mutex);
        /*
         * create related quantum. we will release quantum in scull_release
         * 1. find scull_qset from list; 2. create qset; 3. create quantum.
         */
        for (i = 0; i <= qset_n; i++) {
                if (iter_qset->next == NULL) {
                        /* create struct scull_qset; */
                        p_qset = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
                        if (!p_qset) {
                                ASSERT(0);
                                return -ENOMEM;
                        }
                        memset(p_qset, 0, sizeof(struct scull_qset));

                        iter_qset->next = p_qset;
                        p_qset->next = NULL;
                }
                        
                iter_qset = iter_qset->next;
        }

        /* iter_qset->data = create qset */
        if (!iter_qset->data) {
                iter_qset->data = kmalloc(sizeof(char *) * scull_dev->qset, GFP_KERNEL);
                if (iter_qset->data == NULL) {
                        ASSERT(0);
                        return -ENOMEM;
                }
                memset(iter_qset->data, 0, sizeof(char *) * scull_dev->qset);
        }

        /* data[quantum] = create quantum */
        if (!iter_qset->data[quantum_n]) {
                iter_qset->data[quantum_n] = kmalloc(sizeof(char) * scull_dev->quantum, GFP_KERNEL);
                if (iter_qset->data[quantum_n] == NULL) {
                        ASSERT(0);
                        return -ENOMEM;
                }
                memset(iter_qset->data[quantum_n], 0, sizeof(char) * scull_dev->quantum);
        }

        /* copy data from userspace buffer */
        result = copy_from_user(iter_qset->data[quantum_n] + count, buf, write_bytes);
        if (result < 0) {
                pr_err("scull: can not copy data from user space\n");
                return result;
        }

        scull_dev->size = scull_dev->size + write_bytes;

        /* update off */
        *off = off_n + write_bytes;

        mutex_unlock(&scull_dev->mutex);

        /* error handle */

        printk(KERN_ALERT "scull: write\n");

        /* return number of bytes we wrote */
        return write_bytes;
}

/* just try to use ioctl, here using ioctl to modify qset/quantum is a stupid design */
long scull_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
        /* can not support multi-process */
        int tmp;
        struct uapi_parameter prmt;
        struct scull_dev *scull_dev = file->private_data;

        switch (cmd) {
        case SCULL_SET_QSET:
                qset = arg;
                scull_dev->qset = arg;
                break;
        case SCULL_SET_QUANTUM:
                quantum = arg;
                scull_dev->quantum = arg;
                break;
        case SCULL_SET_QSET_Q:
                /* int __user * */
                if (!get_user(tmp, (int __user *)arg)) {
                        qset = tmp;
                        scull_dev->qset = tmp;
                        break;
                } else
                        return -EFAULT;
        case SCULL_SET_QUANTUM_Q:
                if (!get_user(tmp, (int __user *)arg)) {
                        quantum = tmp;
                        scull_dev->quantum = tmp;
                        break;
                } else
                        return -EFAULT;
        case SCULL_SET_STRUCT_Q:
                if (!copy_from_user(&prmt, (struct uapi_parameter *)arg, sizeof(prmt))) {
                        qset = prmt.qset;
                        quantum = prmt.quantum;
                        scull_dev->qset = qset;
                        scull_dev->quantum = quantum;
                        break;
                } else
                        return -EFAULT;
        case SCULL_GET_QSET:
                /* we can do nothing here, must use point */
        case SCULL_GET_QUANTUM:
                /* we can do nothing here, must use point */
                break;
        case SCULL_GET_QSET_Q:
                if (put_user(qset, (int *)arg))
                        return -EFAULT;
                break;
        case SCULL_GET_QUANTUM_Q:
                if (put_user(quantum, (int *)arg))
                        return -EFAULT;
                break;
        case SCULL_GET_STRUCT_Q:
                prmt.qset = qset;
                prmt.quantum = quantum;
                if (copy_to_user(((struct uapi_parameter *)arg), &prmt, sizeof(prmt)))
                        return -EFAULT;
                break;
        default:
                pr_err("scull: invalid ioctl command!\n");
                return -EINVAL;
        }

        return 0;
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
        return 0;
}

int scull_mmap(struct file *file, struct vm_area_struct *vma)
{
        unsigned long page = virt_to_phys(scull_device->mmap_memory);
        unsigned long start = (unsigned long)vma->vm_start;
        unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);

        if (remap_pfn_range(vma, start, page >> PAGE_SHIFT, size, PAGE_SHARED)) {
                printk(KERN_ALERT "remap_pfn_range failed!\n");
                return -1;
        }

        return 0;
}

struct file_operations scull_fops = {
        .owner = THIS_MODULE,
        .llseek = NULL,
        .read = scull_read,
        .write = scull_write,
        .unlocked_ioctl = scull_ioctl,
        .open = scull_open,
        .release = scull_release,
        .mmap = scull_mmap,
};

static int __init scull_init(void)
{
        dev_t dev_id;
        int err = 0;
        unsigned int firstminor = 0;
        unsigned int count = 1;
        char *dev_name = "scull";

        /* create scull_dev */
        scull_device = kmalloc(sizeof(struct scull_dev), GFP_KERNEL);
        if (!scull_device) {
                ASSERT(0);
                return -ENOMEM;
        }
        /*
         * offer different ways to set qset and quantum: now we have default
         * value, and we can set them by module parametres.
         */
        scull_device->head.data = NULL;
        scull_device->head.next = NULL;
        scull_device->qset = qset;
        scull_device->quantum = quantum;
        scull_device->size = 0;
        mutex_init(&scull_device->mutex);

        /* alloc dev_id */
        err = alloc_chrdev_region(&dev_id, firstminor, count, dev_name);
        if (err < 0) {
                /* can we use dev_err here? It seems no struct device for a cdev */
                pr_err("scull: can not allocate a cdev\n");
                return err;
        }

        /* register cdev */
        cdev_init(&scull_device->cdev, &scull_fops);
        err = cdev_add(&scull_device->cdev, dev_id, count);
        if (err < 0) {
                pr_err("scull: can not add a cdev to system\n");
                return err;
        }

        scull_class = class_create(THIS_MODULE, dev_name);
        if (IS_ERR(scull_class)) {
                pr_err("scull: fail to create device %s\n", dev_name);
                return -1;
        }
        device_create(scull_class, NULL, dev_id, NULL, "scull" "%d", 0);

        scull_device->mmap_memory = kzalloc(64, GFP_KERNEL);
        scull_device->mmap_memory[0] = "0";
        scull_device->mmap_memory[1] = "1";
        scull_device->mmap_memory[2] = "2";

        return 0;
}

static void __exit scull_exit(void)
{
        int i;
        struct scull_dev *scull_dev = scull_device;
        dev_t dev_id = scull_dev->cdev.dev;
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
                        iter_qset = iter_qset->next;
                }
        }

        /* free scull_qset list */
        iter_qset = &scull_dev->head;
        scull_free_qset_list(iter_qset);

        device_destroy(scull_class, dev_id);
        class_destroy(scull_class);

        /* remove cdev before free scull_device */
        cdev_del(&scull_dev->cdev);

        printk(KERN_ALERT "%s\n", scull_device->mmap_memory);
        kfree(scull_device->mmap_memory);

        /* free scull_dev */
        kfree(scull_device);

        printk(KERN_ALERT "Goodbye, cruel world\n");
}

module_init(scull_init);
module_exit(scull_exit);

MODULE_AUTHOR("Sherlock");
MODULE_DESCRIPTION("The driver for scull Module");
MODULE_ALIAS("The driver for scull Module");
