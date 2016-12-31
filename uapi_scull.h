#ifndef UAPI_SCULL_H
#define UAPI_SCULL_H

#define _IOC_NRBITS	8
#define _IOC_TYPEBITS	8
#define _IOC_SIZEBITS	14
#define _IOC_DIRBITS	2

#define _IOC_NRSHIFT	0
#define _IOC_TYPESHIFT	(_IOC_NRSHIFT+_IOC_NRBITS)
#define _IOC_SIZESHIFT	(_IOC_TYPESHIFT+_IOC_TYPEBITS)
#define _IOC_DIRSHIFT	(_IOC_SIZESHIFT+_IOC_SIZEBITS)

#define _IOC(dir,type,nr,size)             \
	 (((dir)  << _IOC_DIRSHIFT) |      \
	 ((type) << _IOC_TYPESHIFT) |      \
	 ((nr)   << _IOC_NRSHIFT) |        \
	 ((size) << _IOC_SIZESHIFT))

#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),(_IOC_TYPECHECK(size)))

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

struct uapi_parameter {
        int qset;
        int quantum;
};

#endif /* UAPI_SCULL_H */
