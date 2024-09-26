#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    
	int socket_fd;
    struct sockaddr_in server_addr;
    char message[2000], server_reply[2000];
	
	// create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        printf("Failed to create socket\n");
        return 1;
    }

    // setting socket
	server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = inet_addr("140.113.149.60");
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons((u_short)atoi(argv[2]));
	// server_addr.sin_port = htons(4444);

    // connect to server
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("connect failed\n");
        return 1;
    }

    // printf("connect success\n");
    
    sprintf(message, "%s|%s|%s", argv[3], argv[4], argv[5]);
    // printf("%s\n", message);

    send(socket_fd, message, strlen(message), 0);
        
    return 0;
}