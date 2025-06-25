#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PORT 8080
#define BUFF_SIZE 16

struct data_for_handler {
    struct sockaddr_in client;
    int fd;
    int is_ready;
};

void* handler(void* args);

int main() {
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
        exit(EXIT_FAILURE);
    }
    if(bind(fd, (struct sockaddr*)&server, size_server) == -1) {
        perror("Bind fail\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //Threads preparation
    struct data_for_handler data[3];
    data[0].fd = data[1].fd = data[2].fd = fd;
    data[0].is_ready = data[1].is_ready = data[2].is_ready = 0;

    pthread_t threads[3];
    pthread_create(&threads[0], NULL, handler, (void*)&data[0]);
    pthread_create(&threads[1], NULL, handler, (void*)&data[1]);
    pthread_create(&threads[2], NULL, handler, (void*)&data[2]);
    pthread_detach(threads[0]);
    pthread_detach(threads[1]);
    pthread_detach(threads[2]);

    //Main cycle
    printf("Server is working...\n");
    printf("Listening...\n");
    char buff[BUFF_SIZE];
    while(1) {
        int free_thread;
        for(int i = 0; i < 3; i++) {
            if(data[i].is_ready == 0) {
                free_thread = i;
                break;
            }
            if(i == 2) {
                usleep(100000);
                i = 0;
            }
        }

        size_client = sizeof(client);
        if(recvfrom(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)&client, &size_client) == -1) {
            perror("Recv fail\n");
            data[0].is_ready = data[1].is_ready = data[2].is_ready = -1;
            sleep(1);
            close(fd);
            exit(EXIT_FAILURE);
        }

        if(strcmp(buff, "kill") == 0) {
            printf("Received a kill message\n");
            data[0].is_ready = data[1].is_ready = data[2].is_ready = -1;
            sleep(1);
            break;
        }

        data[free_thread].client = client;
        data[free_thread].is_ready = 1;
    }
    
    close(fd);
    return 0;
}

void* handler(void* args) {
    struct data_for_handler* data = (struct data_for_handler*) args;
    char buff[16];

    time_t rawtime;
    struct tm *timeinfo;
    
    while(1) {
        if(data->is_ready == 0) {
            usleep(100000);
            continue;
        } else if (data->is_ready == -1) {
            break;
        }

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        sprintf(buff, "%02d:%02d:%02d\n", 
                timeinfo->tm_hour, 
                timeinfo->tm_min, 
                timeinfo->tm_sec);

        if(sendto(data->fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&data->client, sizeof(data->client)) == -1) {
            perror("Send fail");
            close(data->fd);
            exit(EXIT_FAILURE);
        }
        data->is_ready = 0;
    }
    return NULL;
}