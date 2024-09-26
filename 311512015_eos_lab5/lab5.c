#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //dup2
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

int server_fd, client_fd;

void handler(int signum) {
    close(server_fd);
	exit(0);
}

void zombie_handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char *argv[])
{
	// set signal
	signal(SIGINT, handler);
	signal(SIGCHLD, zombie_handler);
	
	// create a socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0){
		printf("Fail to create a socket.");
		exit(0);
	}

	int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	// set socket attribute
	struct sockaddr_in ser_addr, client_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = INADDR_ANY;
    ser_addr.sin_port = htons((u_short)atoi(argv[1]));
	// ser_addr.sin_port = htons(4444);

	// socket binding
	if (bind(server_fd, (const struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
		printf("Bind socket failed!");
		close(server_fd);
		exit(0);
	}
	// printf("Socket success\n");

    // socket listening
    listen(server_fd, 5);
    // printf("waiting...\n");

	int slen = sizeof(client_addr);
	while(1){
    	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&slen);
		// printf("success\n");

		int pid = fork();
		if (pid == 0) {
			dup2(client_fd, STDOUT_FILENO);
			execlp("sl", "sl", "-l", NULL);
            exit(0);
        } else if (pid < 0) {
            printf("child process failed\n");
            return 1;
        }
		else{
			printf("Train ID: %d\n", pid);
		}
	}
    
    return 0;
}