#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/timerfd.h>
#include <sys/select.h>

#define MAX_DRIVERS 10
#define BUFF_SIZE 30

struct msg {
    int time;
    int stat;
};

struct drivers {
    int pipes[MAX_DRIVERS*2][2];
    pid_t pids[MAX_DRIVERS];
    int ndrivers;
    int last_index;
};

int finf_index(struct drivers* drv, pid_t pid) {
    int i = 0;
    for(; i < drv->ndrivers; i++) {
        if(drv->pids[i] == pid) {
            return i;
        }
    }
    return -1;
}

void driver_handler(int*, int*);

int main() {
    struct drivers drv;
    drv.ndrivers = 0;
    drv.last_index = 0;
    char buff[BUFF_SIZE];

    printf("COMMANDS:\ncreate_driver\nsend_task <pid> <task_timer>\nget_status <pid>\nget_drivers\nquit\n\n");

    while(1) {
        printf(">");
        memset(buff, '\0', BUFF_SIZE);
        fgets(buff, BUFF_SIZE, stdin);
        buff[strlen(buff)+1] = '\0';
        char* token = strtok(buff, "\n");
        if(strcmp(token, "create_driver") == 0) {
            if(drv.ndrivers == MAX_DRIVERS){
                printf("Already max drivers\n\n");
                continue;
            }
            pipe(drv.pipes[drv.ndrivers*2]);
            pipe(drv.pipes[drv.ndrivers*2+1]);
            int ind = drv.last_index++;
            pid_t pid = fork();
            if(pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if(pid == 0) {
                driver_handler(drv.pipes[ind*2], drv.pipes[ind*2+1]);
                return 0;
            } else {
                drv.pids[drv.ndrivers] = pid;
                close(drv.pipes[drv.ndrivers*2][0]);
                close(drv.pipes[drv.ndrivers*2+1][1]);
                drv.ndrivers++;
                printf("Driver %d created\n\n", pid);
            }
            continue;
        } else if(strcmp(token, "quit") == 0) {
            struct msg m;
            m.stat = 3;
            m.time = 0;
            int res;
            for(int i = 0; i < drv.ndrivers; i++) {
                write(drv.pipes[i*2][1], &m, sizeof(struct msg));
            }
            for(int i = 0; i < drv.ndrivers; i++) {
                close(drv.pipes[i*2][1]);
                close(drv.pipes[i*2+1][0]);
                waitpid(drv.pids[i], &res, 0);
            }
            break;
        } else if(strcmp(token, "get_drivers") == 0) {
            struct msg m;
            for(int i = 0; i < drv.ndrivers; i++) {
                m.stat = 2;
                m.time = 0;
                write(drv.pipes[i*2][1], &m, sizeof(struct msg));
                read(drv.pipes[i*2+1][0], &m, sizeof(struct msg));
                char status[10];
                if(m.stat == 0) {
                    strcpy(status, "Available");
                } else {
                    strcpy(status, "Busy");
                }
                printf("Driver %d - %s. ", drv.pids[i], status);
                if(m.stat == 1) {
                    printf("Time: %dsec\n", m.time);
                }
            }
            printf("\n\n");
            continue;
        }
        token = strtok(buff, " ");
        if(strcmp(token, "send_task") == 0) {
            token = strtok(NULL, " ");
            int index = finf_index(&drv, atoi(token));
            token = strtok(NULL, " ");
            struct msg m;
            m.time = atoi(token);
            write(drv.pipes[index*2][1], &m, sizeof(struct msg));
            continue;
        } else if(strcmp(token, "get_status") == 0) {
            token = strtok(NULL, " ");
            int index = finf_index(&drv, atoi(token));
            struct msg m;
            m.stat = 2;
            m.time = 0;
            write(drv.pipes[index*2][1], &m, sizeof(struct msg));
            read(drv.pipes[index*2+1][0], &m, sizeof(struct msg));
            char status[10];
            if(m.stat == 0) {
                strcpy(status, "Available");
            } else {
                strcpy(status, "Busy");
            }
            printf("Driver %d - %s. ", drv.pids[index], status);
            if(m.stat == 1) {
                printf("Time: %dsec\n", m.time);
            }
            printf("\n\n");
        }
    }
}

void driver_handler(int* in, int* out) {
    sleep(1);
    close(in[1]);
    close(out[0]);
    int status = 0;
    int timefd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    struct itimerspec timer = {
        .it_value = { .tv_sec = 0, .tv_nsec = 0 },
        .it_interval = { .tv_sec = 0, .tv_nsec = 0 }
    };
    fd_set fds;
    struct timeval tv;
    struct msg buff;

    while(1) {
        FD_ZERO(&fds);
        FD_SET(timefd, &fds);
        FD_SET(in[0], &fds);
        int max_fd = (timefd < in[0]) ? in[0] : timefd;

        tv.tv_sec = 0;
        tv.tv_usec = 100000; 

        int ready = select(max_fd+1, &fds, NULL, NULL, &tv);
        if(ready == -1) {
            perror("select");
            close(timefd);
            close(in[0]);
            close(out[1]);
            exit(EXIT_FAILURE);
        }
        if(ready != 0) {
            if(FD_ISSET(timefd, &fds)) {
                status = 0;
            }
            if(FD_ISSET(in[0], &fds)) {
                read(in[0], &buff, sizeof(struct msg));
                if(buff.time != 0) {
                    timer.it_value.tv_sec = buff.time;
                    timerfd_settime(timefd, 0, &timer, NULL);
                    status = 1;
                } else if(buff.stat == 2) {
                    buff.stat = status;
                    struct itimerspec tmp;
                    timerfd_gettime(timefd, &tmp);
                    buff.time = tmp.it_value.tv_sec;
                    write(out[1], &buff, sizeof(struct msg));
                } else if(buff.stat == 3) {
                    close(timefd);
                    close(in[0]);
                    close(out[1]);
                    return;
                }
            }
        }
    }
    close(timefd);
    close(in[0]);
    close(out[1]);
}