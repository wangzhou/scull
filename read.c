#include <fcntl.h>  
#include <sys/ioctl.h>  
#include <unistd.h>  

#include "./uapi_scull.h"

int main()  
{  
        int fd;  
        char *path="/dev/scull0";  
        char buff[11];

        fd = open(path, O_RDWR);
        if(fd < 0) {  
                perror("open error\n");  
                return -1;  
        }  

        /* how to export below cmd to user space? */
        ioctl(fd, SCULL_SET_QSET, 2000);
        ioctl(fd, SCULL_SET_QUANTUM, 5000);

        read(fd, buff, 10);  

        close(fd);  
        return 1;  
}  


