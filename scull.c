/* scull.c */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include "scull.h"

MODULE_LICENSE("Dual BSD/GPL");

loff_t scull_llseek(struct file *file, loff_t off, int num)
{
        return 0;
}

ssize_t scull_read(struct file *file, char __user *buf, size_t num, loff_t *off)
{
        return 0;
}

ssize_t scull_write(struct file *file, const char __user *buf, size_t num, loff_t *off)
{
        return 0;
}

int scull_open(struct inode *inode, struct file *file)
{
        return 0;
}

int scull_release(struct inode *inode, struct file *file)
{
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

        scull_dev->head = NULL;
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
