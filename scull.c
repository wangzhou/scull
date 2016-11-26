/* hello.c */
#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("Dual BSD/GPL");

static int __init scull_init(void)
{

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

