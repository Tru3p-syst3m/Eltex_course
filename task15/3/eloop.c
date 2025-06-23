#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
int main() {
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("pid %d\n", getpid());
    fflush(stdout);
    while(1) {
        sigwait(&set, &sig);
        printf("Catch a signal!\n");
    }

    return 0;
}