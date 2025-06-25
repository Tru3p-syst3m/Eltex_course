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
    struct sockaddr_in* client;
    int fd;
};

void* handler(void* args);

int main() {
    //Socet preparation
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
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

    //Main cycle
    printf("Server is working...\n");
    printf("Listening...\n");
    char buff[BUFF_SIZE];
    while(1) {
        struct data_for_handler* data = malloc(sizeof(struct data_for_handler));
        if(!data) {
            perror("Malloc(data_for_handler) fail\n");
            close(fd);
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in* client = malloc(sizeof(struct sockaddr_in));
        if(!client) {
            perror("Malloc(sockaddr_in) fail\n");
            free(data);
            close(fd);
            exit(EXIT_FAILURE);
        }
        size_client = sizeof(*client);
        memset(client, 0, sizeof(size_client));

        if(recvfrom(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)client, &size_client) == -1) {
            perror("Recv fail\n");
            free(data);
            free(client);
            close(fd);
            exit(EXIT_FAILURE);
        }

        if(strcmp(buff, "kill") == 0) {
            printf("Received a kill message\n");
            free(data);
            free(client);
            break;
        }

        pthread_t p;
        data->client = client;
        data->fd = fd;
        pthread_create(&p, NULL, handler, (void*)data);
        pthread_detach(p);
    }
    
    close(fd);
    return 0;
}

void* handler(void* args) {
    struct data_for_handler* data = (struct data_for_handler*) args;
    char buff[16];

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(buff, "%02d:%02d:%02d\n", 
           timeinfo->tm_hour, 
           timeinfo->tm_min, 
           timeinfo->tm_sec);

    if(sendto(data->fd, buff, BUFF_SIZE, 0, (struct sockaddr*)data->client, sizeof(*data->client)) == -1) {
        perror("Send fail");
        close(data->fd);
        free(data->client);
        free(data);
        exit(EXIT_FAILURE);
    }

    free(data->client);
    free(data);
    return NULL;
}