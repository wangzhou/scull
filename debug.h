#ifndef _SCULL_DEBUG_H
#define _SCULL_DEBUG_H

#define ASSERT(x)                                                       \
do {                                                                    \
        if (x) break;                                                   \
        printk(KERN_EMERG "ASSERTION FAILED %s: %s: %d\n",              \
               __FILE__, __func__, __LINE__);                           \
} while (0)

#endif /* _SCULL_DEBUG_H */

