#include <fcntl.h>  
#include <stdio.h>  
#include <sys/ioctl.h>  
#include <sys/mman.h>  
#include <unistd.h>  

#include "./uapi_scull.h"

int main()  
{  
        int fd;  
        int qset;
        int quantum;
        struct uapi_parameter test2_in;
        struct uapi_parameter test2_out;

        char *path="/dev/scull0";  
        char buff[11];

        void *addr;
        int i;

        fd = open(path, O_RDWR);
        if(fd < 0) {  
                perror("open error\n");  
                return -1;  
        }  

        /* how to export below cmd to user space? */
        ioctl(fd, SCULL_SET_QSET, 2000);
        ioctl(fd, SCULL_SET_QUANTUM, 5000);

        ioctl(fd, SCULL_GET_QSET_Q, &qset);
        ioctl(fd, SCULL_GET_QUANTUM_Q, &quantum);

        if (qset == 2000 && quantum == 5000)
                printf("test case 1: qset: %d, quantum: %d\n", qset, quantum);

        test2_in.qset = 6000;
        test2_in.quantum = 7000;
        ioctl(fd, SCULL_SET_STRUCT_Q, &test2_in);
        ioctl(fd, SCULL_GET_STRUCT_Q, &test2_out);

        if (test2_out.qset == 6000 && test2_out.quantum == 7000)
                printf("test case 2: qset: %d, quantum: %d\n", test2_out.qset,
                       test2_out.quantum);

//        read(fd, buff, 10);

        addr = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if(addr == MAP_FAILED)
                printf("mmap fail\n");  

        for(i=0; i<10; i++)  
                printf("%c\n", *((char *)addr + i));

        close(fd);  

        return 1;  
}  


