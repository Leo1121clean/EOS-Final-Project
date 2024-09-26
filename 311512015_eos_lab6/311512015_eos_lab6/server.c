#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //dup2
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>

#define SEM_MODE 0666
#define SEM_KEY 1234


typedef struct {
    char mode[20];
    int amount;
	int times;
	int index;
} recording;

int sem;
int total = 0;

void socket_connetion(char* port);

int server_fd, client_fd;
struct sockaddr_in ser_addr, client_addr;

void socket_connetion(char* port)
{
	// create a socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0){
		printf("Fail to create a socket.");
		exit(0);
	}

	int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	// set socket attribute
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = INADDR_ANY;
    ser_addr.sin_port = htons((u_short)atoi(port));
	// ser_addr.sin_port = htons(4444);

	// socket binding
	if (bind(server_fd, (const struct sockaddr *)&ser_addr, sizeof(ser_addr)) < 0) {
		printf("Bind socket failed!");
		close(server_fd);
		exit(0);
	}

    // socket listening
    listen(server_fd, 3);
}

/* acquire */
int P (int s)
{
    struct sembuf sop;  /* the operation parameters */
    sop.sem_num = 0;    /* access the 1st (and only) sem in the array */
    sop.sem_op = -1;    /* wait..*/
    sop.sem_flg = 0;    /* no special options needed */
    // printf("yes sir\n");
    if (semop (s, &sop, 1) < 0) {
        fprintf(stderr,"P(): semop failed: %s\n",strerror(errno));
        return -1;
    } else {
        return 0;
    }
}

/* release */
int V(int s)
{
    struct sembuf sop;  /*the operation parameters */
    sop.sem_num = 0;    /*the 1st (and only) sem in the array */
    sop.sem_op = 1;     /* signal */
    sop.sem_flg = 0;    /* no special options needed */

    if (semop(s, &sop, 1) < 0) {
        fprintf(stderr,"V(): semop failed: %s\n",strerror(errno));
        return -1;
    } else {
        return 0;
    }
}

void *doing(void *data)
{
	recording* datas = (recording*) data;

	int time = datas->times;
	int amounts = datas->amount;
	char modes[20];
	strcpy(modes, datas->mode);
	// printf("data address %p\n", data);
	
	for(int i=0;i<time;i++){
		P(sem);
		
		// printf("%ld %d\n", pthread_self(), i);
		if(!strcmp(modes, "withdraw")){
			total -= amounts;
			printf("After withdraw: %d\n", total);
		}
		else if(!strcmp(modes, "deposit")){
			total += amounts;
			printf("After deposit: %d\n", total);
		}
		usleep(1000);

		V(sem);
	
	}

	pthread_exit(NULL);
}

void sigint_handler(int sig)
{
	/* remove semaphore */
    if (semctl (sem, 0, IPC_RMID, 0) < 0)
    {
        exit(1);
    }
    printf("Semaphore %d has been remove\n", SEM_KEY);
	
	exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);
	
	/* socket initialization */
	socket_connetion(argv[1]);
	char server_message[2000], client_message[2000];

	/* create semaphore */
    sem = semget(SEM_KEY, 1, IPC_CREAT | SEM_MODE);
    if (sem < 0)
    {
        fprintf(stderr, "Sem %d creation failed: %s\n", SEM_KEY,
            strerror(errno));
        exit(-1);
    }
	/* initial semaphore value to 1 (binary semaphore) */
    if ( semctl(sem, 0, SETVAL, 1) < 0 )
    {
        fprintf(stderr, "Unable to initialize Sem: %s\n", strerror(errno));
        exit(0);
    }
    printf("Semaphore %d has been created & initialized to 1\n", SEM_KEY);

	/* Main Running */
	int slen = sizeof(client_addr);
	char temp[20][100];
	char* p;

	pthread_t threads[6];
	int index = 0,count, rc;

	// recording* record = (recording*)malloc(sizeof(recording));
	recording record;
	
	/* connection loop */
	while(1){
		count = 0;

		// printf("waiting for accept...\n");
    	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&slen);

		memset(temp, 0, sizeof(temp));

		recv(client_fd,client_message,sizeof(client_message),0);
		p = strtok(client_message, "|");
		while(p != NULL){
			strcpy(temp[count], p);
			p = strtok(NULL, "|");
			count++;
		}
		// printf("%s %s %s\n", record[0], record[1], record[2]);

		strcpy(record.mode, temp[0]);
		record.amount = atoi(temp[1]);
		record.times = atoi(temp[2]);
		record.index = index;

		rc = pthread_create(&threads[index], NULL, doing, &record);
		if (rc){
			printf("ERROR; pthread_create() returns %d\n", rc);
			exit(-1);
		}
		rc = pthread_detach(threads[index]);
		index++;
		
		// usleep(1000000);

		memset(client_message, 0, sizeof(client_message));
		close(client_fd);
	}

	
	
	pthread_exit(NULL);
    // return 0;
}