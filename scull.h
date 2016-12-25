#ifndef _SCULL_H
#define _SCULL_H

#include <linux/cdev.h>
#include <linux/printk.h>
#include <linux/mutex.h>
#include <uapi/asm-generic/ioctl.h>

#ifdef SCULL_DEBUG
        #define S_DEBUG(fmt, args...) pr_debug("scull: ", fmt, ## args)
#else
        #define S_DEBUG(fmt, args...)
#endif


int qset = 1000;
int quantum = 4000;

struct scull_qset {
        struct scull_qset *next;
        char **data;
};

struct scull_dev {
        struct scull_qset head;
        int qset;
        int quantum;
        unsigned long size;
        struct cdev cdev;    
        struct mutex mutex;
};

#define SCULL_IOC_MAGIC         '!'

#define SCULL_SET_QSET          _IOW(SCULL_IOC_MAGIC, 0, int)
#define SCULL_SET_QUANTUM       _IOW(SCULL_IOC_MAGIC, 1, int)

#endif /* _SCULL_H */
