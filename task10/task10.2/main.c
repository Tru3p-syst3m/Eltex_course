#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PNUM_CHILDREN 2

void process1();
void process2();

int main() {
    printf("proc0: %d\n", getpid());
    pid_t pids[PNUM_CHILDREN];
    for(int i = 0; i < PNUM_CHILDREN; i++) {
        pids[i] = fork();
        if(pids[i] == -1) {
            perror("Fork faild");
        }else if(pids[i] == 0) {
            switch (i)
            {
            case 0:
                process1();
                return 1;
            
            case 1:
                process2();
                return 2;

            default:
                break;
            }
        }
    }
    int status;
    for(int i = 0; i < PNUM_CHILDREN; i++) {
        waitpid(pids[i], &status, 0);
    }
}

void process1() {
    printf("proc1: %d. parent: %d\n", getpid(), getppid());
    pid_t pids[PNUM_CHILDREN];
    for(int i = 0; i < PNUM_CHILDREN; i++) {
        pids[i] = fork();
        if(pids[i] == -1) {
            perror("Fork faild");
        }else if(pids[i] == 0) {
            switch (i)
            {
            case 0:
                printf("proc3: %d. parent: %d\n", getpid(), getppid());
                exit(3);
            
            case 1:
                printf("proc4: %d. parent: %d\n", getpid(), getppid());
                exit(4);

            default:
                break;
            }
        }
    }
    int status;
    for(int i = 0; i < PNUM_CHILDREN; i++) {
        waitpid(pids[i], &status, 0);
    }
}

void process2() {
    printf("proc2: %d. parent: %d\n", getpid(), getppid());
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork faild");
    } else if(pid == 0) {
        printf("proc5: %d. parent: %d\n", getpid(), getppid());
    }
    int status;
    waitpid(pid, &status, 0);
}