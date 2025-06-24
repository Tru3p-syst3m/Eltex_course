#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 7
#define SOCKET_PORT 8080

int main() {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SOCKET_PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    char buff[BUFF_SIZE];
    strcpy(buff, "Hello!");
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("Socet fail");
        exit(EXIT_FAILURE);
    }

    if(connect(fd, (struct sockaddr*)&server, sizeof(server))) {
        perror("Connect fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Sending a message: %s\n", buff);
    if(send(fd, buff, BUFF_SIZE, 0) == -1) {
        perror("Send fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for answer...\n");
    if(recv(fd, &buff, BUFF_SIZE, 0) == -1) {
        perror("Recv fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Received a message: %s\n", buff);

    close(fd);
    return 0;
}