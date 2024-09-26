#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //dup2
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

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

int main(int argc, char *argv[])
{
	/* socket initialization */
	socket_connetion(argv[1]);
	char server_message[2000], client_message[2000], merge_message[2000];
	char option[100], human[100];
	char record[20][100];

	/* all regions data initialization */
	int data[9][4] = {0};
	int num_temp;

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

	/* Main Running */
	int slen = sizeof(client_addr), i, count = 0;
	int station_index, human_index;
	const char *d_line = "|", *d_space = " ";
    char *p;
	int time, time_temp = 0;
	
	/* connection loop */
	while(1){
		printf("waiting for accept...\n");
    	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t*)&slen);
		
		memset(data, 0, sizeof(data));
		/* UI loop */
		while((recv(client_fd,client_message,sizeof(client_message),0)) != 0){

			/* record input information */
			p = strtok(client_message, d_line);
			while(p != NULL){
				strcpy(record[count], p);
				p = strtok(NULL, d_line);
				count++;
			}
			count = 0;

			strcpy(option, record[0]);
			
			if((strstr(option, "Menu")) != NULL){
				strcpy(server_message, "1. Search\n2. Report\n3. Exit\n");
				send(client_fd,server_message,strlen(server_message)+1,0);
			}
			else if((strstr(option, "Search")) != NULL){
				/* print all station */
				if(strlen(record[1]) == 0){
					for(i=0;i<9;i++){
						sprintf(merge_message, "%s%d\n", search_all[i], data[i][0]);
						strcat(server_message, merge_message);
					}
					send(client_fd,server_message,strlen(server_message)+1,0);
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

					sprintf(merge_message, "Child : %d Adult : %d Elder : %d\n", data[station_index][1], data[station_index][2], data[station_index][3]);
					strcat(server_message, merge_message);
					send(client_fd,server_message,strlen(server_message)+1,0);
				}
			}
			else if((strstr(option, "Report")) != NULL){
				do{
					count++;
					/* decide which station to change */
					if(count%2 == 1){
						if(strstr(record[count], "Baseball Stadium") != NULL)	{station_index=0;}
						else if(strstr(record[count], "Big City") != NULL)	{station_index=1;}
						else if(strstr(record[count], "Neiwan Old Street") != NULL)	{station_index=2;}
						else if(strstr(record[count], "NYCU") != NULL)	{station_index=3;}
						else if(strstr(record[count], "Smangus") != NULL)	{station_index=4;}
						else if(strstr(record[count], "17 km of Splendid Coastline") != NULL)	{station_index=5;}
						else if(strstr(record[count], "100 Tastes") != NULL)	{station_index=6;}
						else if(strstr(record[count], "Science Park") != NULL)	{station_index=7;}
						else if(strstr(record[count], "City God Temple") != NULL)	{station_index=8;}

						sprintf(merge_message, "%s", station_all[station_index]);
						strcat(server_message, merge_message);
						memset(merge_message, 0, sizeof(merge_message));

						if(station_index > time_temp)
							time_temp = station_index;
					}
					/* decide which human to change*/ /* change the number */
					else{
						if(strstr(record[count], "Child") != NULL)	{human_index=1;}
						else if(strstr(record[count], "Adult") != NULL)	{human_index=2;}
						else if(strstr(record[count], "Elder") != NULL)	{human_index=3;}

						strcpy(human, record[count]);
						p = strtok(human, " ");
						p = strtok(NULL, " ");
						num_temp = atoi(p);

						data[station_index][human_index] += num_temp;
						memset(human, 0, sizeof(human));
						// printf("Add station: %d , human: %d, number: %d\n", station_index, human_index, data[station_index][human_index]);

						sprintf(merge_message, " | %s %d\n", human_all[human_index-1], num_temp);
						strcat(server_message, merge_message);
						memset(merge_message, 0, sizeof(merge_message));
					}
				}while((strlen(record[count]) != 0) && (strlen(record[count+1]) != 0));

				/* Respond to client */
				send(client_fd,"Please wait a few seconds...\n",strlen("Please wait a few seconds...\n")+1,0);
				usleep((time_temp + 1) * 1000000);
				time_temp = 0;
				
				count = 0;
				send(client_fd,server_message,strlen(server_message)+1,0);

				for(i=0;i<9;i++)
					data[i][0] = data[i][1] + data[i][2] + data[i][3];
				
			}
			else if((strstr(option, "Exit")) != NULL){
				break;
			}

			memset(server_message, 0, sizeof(server_message));
			memset(client_message, 0, sizeof(client_message));
			memset(merge_message, 0, sizeof(merge_message));
			memset(option, 0, sizeof(option));
			memset(record, 0, sizeof(record));
		}
	}
    return 0;
}