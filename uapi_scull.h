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


#endif /* UAPI_SCULL_H */
