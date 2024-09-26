#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define SHMSZ sizeof(data)

typedef struct {
    int guess;
    char result[8];
}data;

sig_atomic_t answer, shmid;
data *shm, *p;

void game_handler (int signo, siginfo_t *info, void *context)
{
    char response[8];
  
    if(p->guess < answer)
        strcpy(response, "bigger");
    else if(p->guess > answer)
        strcpy(response, "smaller");
    else
        strcpy(response, "bingo");

    strcpy(p->result, response);
    printf("[game] Guess %d, %s\n", p->guess, p->result);
    kill(info->si_pid, SIGUSR1);
}

void signal_handler(int sig)
{
    /* remove shared memory */
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
	
	exit(0);
}

int main (int argc, char **argv)
{
    /* create shared memory */
    key_t key;

    key = atoi(argv[1]);
    shmid = shmget(key, SHMSZ, IPC_CREAT | 0666);
    shm = shmat(shmid, NULL, 0);
    p = shm;

    /* create signal */
    struct sigaction sa;

    memset(&sa, 0, sizeof (sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &game_handler;
    sigaction (SIGUSR1, &sa, NULL);

    signal(SIGINT, signal_handler); // ctrl-c

    answer = atoi(argv[2]);
    printf("[game] Game PID: %d\n", getpid());

    while(1);

    return 0;
}