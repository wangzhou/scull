#include <linux/cdev.h>

int qset = 1000;
int quantum = 4000;

struct scull_qset {
        struct scull_qset *next;
        int **data;
};

struct scull_dev {
        struct scull_qset *head;
        int qset;
        int quantum;
        unsigned long size;
        struct cdev cdev;    
};
