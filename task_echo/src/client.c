#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

#define BUFF_SIZE 1028
#define SERVER_PORT 8080
#define CLIENT_PORT 8081

int main() {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1) {
        perror("Socket fail");
        exit(EXIT_FAILURE);
    }

    while(1) {
        printf(">");
        fgets(buff + sizeof(struct udphdr), BUFF_SIZE - sizeof(struct udphdr), stdin);

        struct udphdr* udp = (struct udphdr*)buff;
        udp->source = htons(CLIENT_PORT);
        udp->dest = htons(SERVER_PORT);
        udp->len = htons(sizeof(struct udphdr) + strlen(buff + sizeof(struct udphdr)) + 1);
        udp->check = 0;

        if(sendto(fd, buff, sizeof(struct udphdr) + strlen(buff + sizeof(struct udphdr)) + 1, 
            0, (struct sockaddr*)&server, sizeof(server)) == -1) {
            perror("Send fail");
            close(fd);
            exit(EXIT_FAILURE);
        }
        if(strcmp(buff + sizeof(struct udphdr), "!exit\n") == 0) {
            break;
        }
        memset(buff, 0, BUFF_SIZE);
        while(1) {
            int size_server = sizeof(server);
            if(recvfrom(fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&server, (socklen_t*)&size_server) == -1) {
                perror("Recv fail");
                close(fd);
                exit(EXIT_FAILURE);
            }

            struct iphdr* ip = (struct iphdr*)buff;
            int ip_len = ip->ihl * 4;
            struct udphdr* recv_udp = (struct udphdr*)(buff + ip_len);
            
            if(ntohs(recv_udp->dest) == CLIENT_PORT) {
                printf("Server: %s\n", buff + ip_len + sizeof(struct udphdr));
                break;
            }
        }
    }
    close(fd);
    return 0;
}