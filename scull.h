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

        /* use to test mmap, init size = 64B */
        char *mmap_memory;
};

struct uapi_parameter {
        int qset;
        int quantum;
};

#define SCULL_IOC_MAGIC         '!'

#define SCULL_SET_QSET          _IOW(SCULL_IOC_MAGIC, 0, int)
#define SCULL_SET_QUANTUM       _IOW(SCULL_IOC_MAGIC, 1, int)
/* *_Q means set/get from point */
#define SCULL_SET_QSET_Q        _IOW(SCULL_IOC_MAGIC, 2, int)
#define SCULL_SET_QUANTUM_Q     _IOW(SCULL_IOC_MAGIC, 3, int)
#define SCULL_SET_STRUCT_Q      _IOW(SCULL_IOC_MAGIC, 4, int)

#define SCULL_GET_QSET          _IOR(SCULL_IOC_MAGIC, 5, int)
#define SCULL_GET_QUANTUM       _IOR(SCULL_IOC_MAGIC, 6, int)
#define SCULL_GET_QSET_Q        _IOR(SCULL_IOC_MAGIC, 7, int)
#define SCULL_GET_QUANTUM_Q     _IOR(SCULL_IOC_MAGIC, 8, int)
#define SCULL_GET_STRUCT_Q      _IOR(SCULL_IOC_MAGIC, 9, int)

#endif /* _SCULL_H */
