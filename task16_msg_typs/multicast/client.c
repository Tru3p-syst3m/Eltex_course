#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFF_SIZE 16
#define SOCKET_PORT 8080
#define MC_IP "224.0.0.1"

int main(int argc, char** argv) {
    struct sockaddr_in client, server;
    memset(&client, 0, sizeof(client));
    memset(&server, 0, sizeof(server));
    client.sin_family = AF_INET;
    client.sin_port = htons(SOCKET_PORT);
    client.sin_addr.s_addr = INADDR_ANY;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("Socet fail\n");
        exit(EXIT_FAILURE);
    }
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MC_IP);
    mreq.imr_interface.s_addr = INADDR_ANY;
    if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt(multy) fail");
        close(fd);
        exit(EXIT_FAILURE);
    }
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(reuse port) fail");
        close(fd);
        exit(EXIT_FAILURE);
    }
    int reuse_ip = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_ip, sizeof(reuse)) < 0) {
        perror("setsockopt(reuse ip) fail");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if(bind(fd, (struct sockaddr*)&client, sizeof(client)) == -1) {
        perror("Bind fail\n");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char buff[BUFF_SIZE];
    int size_server = sizeof(server);
    if(recvfrom(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)&server, (socklen_t*)&size_server) == -1) {
        perror("Recv fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Time from server: %s\n", buff);

    close(fd);
    return 0;
}