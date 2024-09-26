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
#define SEM_KEY1 0011
#define SEM_KEY2 0021
#define SEM_KEY_COUNT 1234
#define SEM_KEY_STAT 0001

/* semaphore declaration */
int sem_car1;
int sem_car2;
int sem_count;
int sem_stat;
int sem_flag[9] = {0};

int data[9][4] = {0}; //global number record

typedef struct
{
    int station;
    int num;
    int wait_time;
} car_state;
car_state car1, car2;

 /* print char */
char search_all[9][50] = {
    "1. Baseball Stadium : ",
    "2. Big City : ",
    "3. Neiwan Old Street : ",
    "4. NYCU : ",
    "5. Smangus : ",
    "6. 17 km of Splendid Coastline : ",
    "7. 100 Tastes : ",
    "8. Science Park : ",
    "9. City God Temple : "
};
char station_all[9][50] = {
    "Baseball Stadium",
    "Big City",
    "Neiwan Old Street",
    "NYCU",
    "Smangus",
    "17 km of Splendid Coastline",
    "100 Tastes",
    "Science Park",
    "City God Temple"
};
char human_all[3][50] = {
    "Child",
    "Adult",
    "Elder"
};

void socket_connetion(char* port);
void waiting_print(int client_indices);

long server_fd;
struct sockaddr_in ser_addr, client_addr;
char client_message[2000];

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
    listen(server_fd, 50);
}

/* acquire */
int P (int s, int i)
{
    struct sembuf sop;  /* the operation parameters */
    sop.sem_num = i;    /* access the 1st (and only) sem in the array */
    sop.sem_op = -1;    /* wait..*/
    sop.sem_flg = 0;    /* no special options needed */

    if (semop (s, &sop, 1) < 0) {
        fprintf(stderr,"P(): semop failed: %s\n",strerror(errno));
        // return -1;
    } else {
        return 0;
    }
}

/* release */
int V(int s, int i)
{
    struct sembuf sop;  /*the operation parameters */
    sop.sem_num = i;    /*the 1st (and only) sem in the array */
    sop.sem_op = 1;     /* signal */
    sop.sem_flg = 0;    /* no special options needed */

    if (semop(s, &sop, 1) < 0) {
        fprintf(stderr,"V(): semop failed: %s\n",strerror(errno));
        // return -1;
    } else {
        return 0;
    }
}

void waiting_print(int client_indices){
    send(client_indices,"The shuttle bus is on it's way...\n",strlen("The shuttle bus is on it's way...\n")+1,0);
}

void *doing(void *client_fd)
{
    /* receive the message */
    long client_index = (long)client_fd;
	char client_messages[2000], server_message[2000], merge_message[2000];;
    // strcpy(client_messages, client_message);
    // printf("index:%ld\n", client_index);
	// printf("input:%s\n", client_messages);


    char record[20][100], option[100];
    char *human;
    char *p;
    const char *d_line = "|";
    int i, j, count = 0;
    int station_index = -2, human_index;
    int time, time_temp = 0;
    int num_temp;
    int data_temp[9][3] = {0}; // record "report numbers"
    int data_aa;
    char *use_msg;

    int time_aa;
    while(1){
        // usleep(100000);
        recv(client_index,client_messages,sizeof(client_messages),0);
        // printf("index:%ld\n", client_index);
        // printf("length: %d %ld\n", data_aa, strlen(client_messages));
        // printf("%d | message: %s\n", ++time_aa, client_messages);
        // printf("%s\n", client_messages);
        
        /* record input information */
        // p = strtok(client_messages, d_line);
        // while(p != NULL){
        //     strcpy(record[count], p);
        //     p = strtok(NULL, d_line);
        //     count++;
        // }
        use_msg = client_messages;
        while ((p = strtok_r(use_msg, "|", &use_msg))) {
            strcpy(record[count++], p);
        }
        count = 0;

        strcpy(option, record[0]);

        if((strstr(option, "Menu")) != NULL){
            // printf("Menu\n");
            strcpy(server_message, "1. Search\n2. Report\n3. Exit\n");
            // printf("%s\n", server_message);
            send(client_index,server_message,strlen(server_message)+1,0);
        }
        else if((strstr(option, "Search")) != NULL){
            // printf("Search\n");
            /* print all station */
            if(strlen(record[1]) == 0){
                for(i=0;i<9;i++){
                    sprintf(merge_message, "%s%d\n", search_all[i], data[i][0]);
                    strcat(server_message, merge_message);
                }
                // printf("%s\n", server_message);
                send(client_index,server_message,strlen(server_message)+1,0);
            }
            /* print a station */
            else{
                if(strstr(record[1], "Baseball Stadium") != NULL)	{station_index=0; strcat(server_message, "Baseball Stadium - ");}
                else if(strstr(record[1], "Big City") != NULL)	{station_index=1; strcat(server_message, "Big City - ");}
                else if(strstr(record[1], "Neiwan Old Street") != NULL)	{station_index=2; strcat(server_message, "Neiwan Old Street - ");}
                else if(strstr(record[1], "NYCU") != NULL)	{station_index=3; strcat(server_message, "NYCU - ");}
                else if(strstr(record[1], "Smangus") != NULL)	{station_index=4; strcat(server_message, "Smangus - ");}
                else if(strstr(record[1], "17 km of Splendid Coastline") != NULL)	{station_index=5; strcat(server_message, "17 km of Splendid Coastline - ");}
                else if(strstr(record[1], "100 Tastes") != NULL)	{station_index=6; strcat(server_message, "100 Tastes - ");}
                else if(strstr(record[1], "Science Park") != NULL)	{station_index=7; strcat(server_message, "Science Park - ");}
                else if(strstr(record[1], "City God Temple") != NULL)	{station_index=8; strcat(server_message, "City God Temple - ");}

                sprintf(merge_message, "Child : %d | Adult : %d | Elder : %d\n", data[station_index][1], data[station_index][2], data[station_index][3]);
                strcat(server_message, merge_message);
                // printf("%s\n", server_message);
                send(client_index,server_message,strlen(server_message)+1,0);
            }
        }
        else if((strstr(option, "Report")) != NULL){
            do{
                count++;
                /* decide which station to change */
                if((strstr(record[count], "Child") == NULL) && (strstr(record[count], "Adult") == NULL) && (strstr(record[count], "Elder") == NULL)){
                    if(strstr(record[count], "Baseball Stadium") != NULL)	{station_index=0;}
                    else if(strstr(record[count], "Big City") != NULL)	{station_index=1;}
                    else if(strstr(record[count], "Neiwan Old Street") != NULL)	{station_index=2;}
                    else if(strstr(record[count], "NYCU") != NULL)	{station_index=3;}
                    else if(strstr(record[count], "Smangus") != NULL)	{station_index=4;}
                    else if(strstr(record[count], "17 km of Splendid Coastline") != NULL)	{station_index=5;}
                    else if(strstr(record[count], "100 Tastes") != NULL)	{station_index=6;}
                    else if(strstr(record[count], "Science Park") != NULL)	{station_index=7;}
                    else if(strstr(record[count], "City God Temple") != NULL)	{station_index=8;}

                    if(station_index > time_temp)
                        time_temp = station_index;
                    
                }
                /* decide which human to change*/ /* change the number */
                else{
                    if(strstr(record[count], "Child") != NULL)	{human_index=1;}
                    else if(strstr(record[count], "Adult") != NULL)	{human_index=2;}
                    else if(strstr(record[count], "Elder") != NULL)	{human_index=3;}

                    // strcpy(human, record[count]);
                    human = record[count];
                    p = strtok_r(human, " ", &human);
                    p = strtok_r(human, " ", &human);
                    // p = strtok(human, " ");
                    // p = strtok(NULL, " ");
                    num_temp = atoi(p);

                    memset(human, 0, sizeof(human));
                    // printf("Add station: %d , human: %d, number: %d\n", station_index, human_index, data[station_index][human_index]);

                    data_temp[station_index][human_index-1] += num_temp;
                }
                
            }while((strlen(record[count]) != 0) && (strlen(record[count+1]) != 0));
            
            // printf("%d %d\n", station_index, sem_flag[station_index]);
            // printf("sem1\n"); printf("de sem1\n")
            if((station_index == 0) && (sem_flag[0] == 1))  {  waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 1) && (sem_flag[1] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 2) && (sem_flag[2] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 3) && (sem_flag[3] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 4) && (sem_flag[4] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 5) && (sem_flag[5] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 6) && (sem_flag[6] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 7) && (sem_flag[7] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else if((station_index == 8) && (sem_flag[8] == 1))  { waiting_print(client_index);  P(sem_stat, station_index); V(sem_stat, station_index); }
            else{

                // printf("sem_stat begin\n");
                P(sem_stat, station_index);
                sem_flag[station_index] = 1; // means this station is in cars or waiting list
                
                
                /* decide which car to choose */
                if(car1.wait_time <= car2.wait_time){ // choose car1
                    car1.wait_time += (time_temp + 1);
                    car1.station = station_index;
                    car1.num = data_temp[station_index][0] + data_temp[station_index][1] + data_temp[station_index][2];

                    waiting_print(client_index);

                    /* wait for car1 */
                    // printf("waiting1....   %ds\n", time_temp+1);
                    P(sem_car1, 0);
                    usleep((time_temp + 1) * 1000000);
                    car1.station = -1;
                    car1.wait_time -= (time_temp + 1);
                    V(sem_car1, 0);
                }
                else{ // choose car2
                    car2.wait_time += (time_temp + 1);
                    car2.station = station_index;
                    car2.num = data_temp[station_index][0] + data_temp[station_index][1] + data_temp[station_index][2];

                    waiting_print(client_index);

                    /* wait for car2 */
                    // printf("waiting2....   %ds\n", time_temp+1);
                    P(sem_car2, 0);
                    usleep((time_temp + 1) * 1000000);
                    car2.station = -1;
                    car2.wait_time -= (time_temp + 1);
                    V(sem_car2, 0);
                }

                // printf("sem_stat end\n");
                V(sem_stat, station_index);
                sem_flag[station_index] = 0;
                
            }

            /* calculation */
            /* server message handling */
            P(sem_count, 0);
            for(i=0;i<9;i++){
                if((data_temp[i][0] + data_temp[i][1] + data_temp[i][2]) > 0){
                    sprintf(merge_message, "%s", station_all[i]);
                    strcat(server_message, merge_message);
                    memset(merge_message, 0, sizeof(merge_message));
                }
                else
                    continue;

                for(j=0;j<3;j++){
                    /* calculation */
                    data[i][j+1] += data_temp[i][j];
                    data[i][0] += data_temp[i][j];
                    
                    if(data_temp[i][j] > 0){
                        sprintf(merge_message, " | %s %d", human_all[j], data_temp[i][j]);
                        strcat(server_message, merge_message);                        
                        memset(merge_message, 0, sizeof(merge_message));
                    }
                }
                strcat(server_message, "\n"); 
            }
            V(sem_count, 0);
            
            /* reset value */
            time_temp = 0;
            count = 0;
            memset(data_temp, 0, sizeof(data_temp));
            
            // printf("%s", server_message);
            // printf("\n");
            send(client_index,server_message,strlen(server_message)+1,0);

        }
        else if((strstr(option, "Exit")) != NULL){
            printf("Exit\n");
            close(client_index);
            break;
        }

        memset(server_message, 0, sizeof(server_message));
        memset(client_messages, 0, sizeof(client_messages));
        memset(merge_message, 0, sizeof(merge_message));
        memset(option, 0, sizeof(option));
        memset(record, 0, sizeof(record));
    }

    printf("ggggggggggg    index: %ld\n", client_index);
	pthread_exit(NULL);
}

void sigint_handler(int sig)
{
    int total = 0, child = 0, adult = 0, elder = 0;
     
	/* remove semaphore */
    semctl (sem_car1, 0, IPC_RMID, 0);
    semctl (sem_car2, 0, IPC_RMID, 0);
    semctl (sem_count, 0, IPC_RMID, 0);
    semctl (sem_stat, 0, IPC_RMID, 0);

    for(int i=0;i<9;i++){
        total += data[i][0];
        child += data[i][1];
        adult += data[i][2];
        elder += data[i][3];
    }

    /* write into txt file */
    FILE* fp;
    fp = fopen("result.txt", "w+");
    fprintf(fp, "Total : %d\n", total);
    fprintf(fp, "Child : %d\n", child);
    fprintf(fp, "Adult : %d\n", adult);
    fprintf(fp, "Elder : %d", elder);

	fclose(fp);
	exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sigint_handler);
	
	/* socket initialization */
	socket_connetion(argv[1]);

	/* create semaphore */
    sem_car1 = semget(SEM_KEY1, 1, IPC_CREAT | SEM_MODE);
    sem_car2 = semget(SEM_KEY2, 1, IPC_CREAT | SEM_MODE);
    sem_count = semget(SEM_KEY_COUNT, 1, IPC_CREAT | SEM_MODE);
    sem_stat = semget(SEM_KEY_STAT, 9, IPC_CREAT | SEM_MODE);

    /*  initialize semaphore*/
    semctl(sem_car1, 0, SETVAL, 1);
    semctl(sem_car2, 0, SETVAL, 1);
    semctl(sem_count, 0, SETVAL, 1);

    for(int i=0;i<9;i++)
        semctl(sem_stat, i, SETVAL, 1);

    /* initialize car state */
    car1.station = -1;
    car2.station = -1;
    car1.num = 0;
    car2.num = 0;
    car1.wait_time = 0;
    car2.wait_time = 0;

	/* Main Running */
	pthread_t threads[100];
	int index = 0, rc;
    int slen = sizeof(client_addr);

	long client_fd;
	/* connection loop */
	while(1){

		// printf("waiting for accept...\n");
    	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&slen);
        
        // usleep(100000);
        // printf("%d\n", index);
		rc = pthread_create(&threads[index], NULL, doing, (void *)client_fd);
		if (rc){
			printf("ERROR; pthread_create() returns %d\n", rc);
			exit(-1);
		}
		rc = pthread_detach(threads[index]);
		index++;
        
		// close(client_fd);
	}
	
	pthread_exit(NULL);
    // return 0;
}