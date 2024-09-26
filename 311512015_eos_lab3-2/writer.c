#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> //file processing
#include <unistd.h> //write function
#include <string.h> 

int main(int argc, char *argv[])
{
    int fd;
    char data[strlen(argv[1]) + 1];
    strcpy(data, argv[1]);
    size_t data_size = sizeof(data);
    
    fd = open("/dev/etx_device", O_WRONLY); //write only
    if (fd < 0) {
        printf("Failed to open the device\n");
        return -1;
    }

    //write student ID in user space(/dev/etx_device)
    if (write(fd, data, data_size) != data_size) {
        printf("Failed to write to the device\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
