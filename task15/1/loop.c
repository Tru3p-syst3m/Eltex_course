#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void sig_handler(int sig, siginfo_t* act, void* args) {
    printf("Got a signal %d!\n", sig);
}

int main() {
    struct sigaction act;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    act.sa_sigaction = sig_handler;
    act.sa_mask = set;

    if(sigaction(SIGUSR1, &act, NULL)) {
        perror("can't set handler\n");
        exit(1);
    }

    printf("pid %d\n", getpid());

    while(1) {
        usleep(500000);
    }

    return 0;
}