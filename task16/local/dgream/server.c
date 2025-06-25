#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 7
#define SOCKET_PATH "server_socket"

int main() {
    struct sockaddr_un server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));
    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, SOCKET_PATH, sizeof(server.sun_path) - 1);
    socklen_t size_server = sizeof(server), size_client;
    
    char buff[BUFF_SIZE];
    
    int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
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

    if (client.sun_family != AF_LOCAL) {
        fprintf(stderr, "Invalid client address\n");
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
    unlink(SOCKET_PATH);
    return 0;
}

