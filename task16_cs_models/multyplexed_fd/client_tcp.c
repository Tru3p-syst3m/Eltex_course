#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 16
#define SOCKET_PORT 8081

int main(int argc, char** argv) {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SOCKET_PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("Socet fail\n");
        exit(EXIT_FAILURE);
    }
    if(connect(fd, (struct sockaddr*)&server, sizeof(server))) {
        perror("Connect fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char buff[BUFF_SIZE];
    buff[BUFF_SIZE] = '\0';
    //If a kill client
    if(argc != 1) {
        printf("Sending a kill message\n");
        strcpy(buff, argv[1]);
        if(send(fd, buff, BUFF_SIZE, 0) == -1) {
            perror("Send fail\n");
            close(fd);
            exit(EXIT_FAILURE);
        }
        close(fd);
        return 0;
    }

    if(send(fd, buff, BUFF_SIZE, 0) == -1) {
        perror("Send fail\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if(recv(fd, &buff, BUFF_SIZE, 0) == -1) {
        perror("Recv fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Time from server: %s\n", buff);

    close(fd);
    return 0;
}