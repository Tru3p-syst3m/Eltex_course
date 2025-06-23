#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

int main(int argc, char** argv) {
    if(argc != 2) {
        printf("Try ./kill_func <pid>\n");
    }

    pid_t pid = (pid_t)atoi(argv[1]);

    kill(pid, SIGUSR1);

    return 0;
}