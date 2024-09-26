#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define SHMSZ sizeof(data)

typedef struct {
    int guess;
    char result[8];
}data;

data *shm, *p;
sig_atomic_t upper_bound,lower_bound, guess;
sig_atomic_t gameid;

void guess_handler (int signo, siginfo_t *info, void *context)
{
    /* update the bounds */
    if(!strcmp(p->result, "smaller"))
        upper_bound = (upper_bound + lower_bound)/2;
    else if(!strcmp(p->result, "bigger"))
        lower_bound = (upper_bound + lower_bound)/2;
    else if(!strcmp(p->result, "bingo"))
        exit(0);
}

void timer_handler (int signum)
{
    /* guess the number */
    guess = (upper_bound + lower_bound)/2;
    p->guess = guess;
    printf("[game] Guess: %d\n", p->guess);
    kill(gameid, SIGUSR1);
}

int main (int argc, char **argv)
{
    gameid = atoi(argv[3]);
    upper_bound = atoi(argv[2]);
    lower_bound = 0;

    /* create shared memory */
    int shmid;
    key_t key;

    key = atoi(argv[1]);
    shmid = shmget(key, SHMSZ, IPC_CREAT | 0666);
    shm = shmat(shmid, NULL, 0);
    p = shm;

    /* create signal */
    struct sigaction sa;

    memset(&sa, 0, sizeof (sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &guess_handler;
    sigaction (SIGUSR1, &sa, NULL);

    /* create timer */
    struct sigaction sa_time;
    struct itimerval timer;

    memset (&sa_time, 0, sizeof (sa_time));
    sa_time.sa_handler = &timer_handler;
    sigaction (SIGVTALRM, &sa_time, NULL);

    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    setitimer (ITIMER_VIRTUAL, &timer, NULL);

    while(1);

    shmdt(shm);
    return 0;
}