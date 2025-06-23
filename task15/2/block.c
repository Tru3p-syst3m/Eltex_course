#include <signal.h>
#include <unistd.h>
#include <stdio.h>
int main() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, NULL);
    printf("pid %d\n", getpid());
    fflush(stdout);
    while(1) {
        sleep(1);
    }
    return 0;
}