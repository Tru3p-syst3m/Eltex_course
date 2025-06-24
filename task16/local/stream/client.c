#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 7
#define SOCKET_PATH "server_socket"

int main() {
    struct sockaddr_un server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));
    server.sun_family = client.sun_family = AF_LOCAL;
    strncpy(server.sun_path, SOCKET_PATH, sizeof(server.sun_path) - 1);
    strncpy(client.sun_path, "client_socket", sizeof(client.sun_path) - 1);
    
    char buff[BUFF_SIZE];
    strcpy(buff, "Hello!");
    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("Socet fail");
        exit(EXIT_FAILURE);
    }

    if(bind(fd, (struct sockaddr*)&client, sizeof(client)) == -1) {
        perror("Bind fail");
        close(fd);
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
    unlink(client.sun_path);
    return 0;
}