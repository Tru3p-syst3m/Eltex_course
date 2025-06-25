#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define SOCKET_PORT 8080
#define BUFF_SIZE 16
#define MQNAME "/mq"

struct data_for_handler {
    struct sockaddr_in client;
    int fd;
    int is_ready;
};

struct message {
    long mtype;
    struct data_for_handler data;
};

void thread_killer(int*, struct message*);

void* handler(void* args);

int main() {
    //Queue preparation
    key_t key = ftok(".", 'A');
    if(key == -1) {
        perror("Ftok fail\n");
        exit(1);
    }
    int mqid = msgget(key, 0600|IPC_CREAT);
    if(mqid == -1) {
        perror("Msgget fail\n");
        exit(1);
    }

    //Socet preparation
    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));
    server.sin_family = AF_INET;
    server.sin_port = htons(SOCKET_PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    socklen_t size_server = sizeof(server), size_client;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("Socet fail\n");
        msgctl(mqid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    if(bind(fd, (struct sockaddr*)&server, size_server) == -1) {
        perror("Bind fail\n");
        msgctl(mqid, IPC_RMID, NULL);
        close(fd);
        exit(EXIT_FAILURE);
    }

    //Threads preparation
    pthread_t threads[3];
    pthread_create(&threads[0], NULL, handler, (void*)&mqid);
    pthread_create(&threads[1], NULL, handler, (void*)&mqid);
    pthread_create(&threads[2], NULL, handler, (void*)&mqid);
    pthread_detach(threads[0]);
    pthread_detach(threads[1]);
    pthread_detach(threads[2]);

    //Main cycle
    printf("Server is working...\n");
    printf("Listening...\n");
    char buff[BUFF_SIZE];
    struct message msg;
    msg.mtype = 1;
    msg.data.fd = fd;
    msg.data.is_ready = 1;
    while(1) {
        size_client = sizeof(client);
        if(recvfrom(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)&client, &size_client) == -1) {
            perror("Recv fail\n");
            thread_killer(&mqid, &msg);
            close(fd);
            sleep(1);
            msgctl(mqid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }

        if(strcmp(buff, "kill") == 0) {
            printf("Received a kill message\n");
            thread_killer(&mqid, &msg);
            sleep(1);
            msgctl(mqid, IPC_RMID, NULL);
            break;
        }

        msg.data.client = client;
        msgsnd(mqid, &msg, sizeof(msg), 0);
    }
    
    close(fd);
    msgctl(mqid, IPC_RMID, NULL);
    return 0;
}

void* handler(void* args) {
    int* mqid = (int*) args;
    char buff[16];

    time_t rawtime;
    struct tm *timeinfo;
    
    while(1) {
        struct message msg;
        if(msgrcv(*mqid, &msg, sizeof(msg), 0, 0) == -1) {
            perror("Msgrcv fail");
            close(msg.data.fd);
            thread_killer(mqid, &msg);
            sleep(1);
            msgctl(*mqid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }

        if (msg.data.is_ready == -1) {
            break;
        }

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        sprintf(buff, "%02d:%02d:%02d\n", 
                timeinfo->tm_hour, 
                timeinfo->tm_min, 
                timeinfo->tm_sec);

        if(sendto(msg.data.fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&msg.data.client, sizeof(msg.data.client)) == -1) {
            perror("Send fail");
            close(msg.data.fd);
            thread_killer(mqid, &msg);
            sleep(1);
            msgctl(*mqid, IPC_RMID, NULL);
            exit(EXIT_FAILURE);
        }
        msg.data.is_ready = 0;
    }
    return NULL;
}

void thread_killer(int* mqid, struct message* msg) {
    msg->data.is_ready = -1;
    msgsnd(*mqid, msg, sizeof(*msg), 0);
    msgsnd(*mqid, msg, sizeof(*msg), 0);
    msgsnd(*mqid, msg, sizeof(*msg), 0);
}
