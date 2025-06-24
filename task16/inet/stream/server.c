#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 7
#define SOCKET_PORT 8080

int main() {
    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SOCKET_PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    char buff[BUFF_SIZE];
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("Socet fail");
        exit(EXIT_FAILURE);
    }

    if(bind(fd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Bind fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is working...\n");

    listen(fd, 5);
    printf("Listening...\n");
    socklen_t size_client = sizeof(client);
    int client_fd = accept(fd, (struct sockaddr*)&client, &size_client);
    if(recv(client_fd, &buff, BUFF_SIZE, 0) == -1) {
        perror("Recv fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Server received a message: %s\n", buff);
    buff[0] = '!';

    printf("Server sending a message: %s\n", buff);
    if(send(client_fd, &buff, BUFF_SIZE, 0) == -1) {
        perror("Send fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}