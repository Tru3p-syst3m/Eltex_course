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
    memset(&server, 0, sizeof(client));
    server.sin_family = AF_INET;
    server.sin_port = htons(SOCKET_PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    socklen_t size_server = sizeof(server), size_client;
    
    char buff[BUFF_SIZE];
    
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("Socet fail");
        exit(EXIT_FAILURE);
    }

    if(bind(fd, (struct sockaddr*)&server, size_server) == -1) {
        perror("Bind fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is working...\n");

    size_client = sizeof(client);
    printf("Listening...\n");
    if(recvfrom(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)&client, &size_client) == -1) {
        perror("Recv fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Server received a message: %s\n", buff);
    buff[0] = '!';

    printf("Server sending a message: %s\n", buff);
    if(sendto(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)&client, size_client) == -1) {
        perror("Send fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}

