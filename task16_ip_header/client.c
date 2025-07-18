#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/udp.h>
#include <netinet/ip.h>


#define BUFF_SIZE 7
#define SOCKET_PORT 8080

int main() {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SOCKET_PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    
    char buff[20 + 8 + BUFF_SIZE];
    strcpy(buff + 8 + 20, "Hello!");

    struct iphdr* ip = (struct iphdr*)buff;
    ip->version = 4;
    ip->ihl = 5;
    ip->tos = 0;
    //length is auto
    ip->id = 0; //auto
    ip->frag_off = 0;
    ip->ttl = 10;
    ip->protocol = IPPROTO_UDP;
    //checksum is auto
    ip->saddr = 0; //auto
    ip->daddr = htonl(INADDR_LOOPBACK);

    unsigned int ip_len = ip->ihl * 4;
    struct udphdr* udp = (struct udphdr*)(buff + ip_len);
    udp->source = htons(8081);
    udp->dest = htons(8080);
    udp->len = htons(15);
    udp->check = htons(0);

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1) {
        perror("Socet fail");
        exit(EXIT_FAILURE);
    }
    int flag = 1; 
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &flag, sizeof(flag));

    printf("Sending a message: %s\n", buff+8+20);
    if(sendto(fd, buff, BUFF_SIZE+8+20, 0, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Send fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for answer...\n");
    char buff2[BUFF_SIZE+8+20];
    while(1) {
        int size_server = sizeof(server);
        if(recvfrom(fd, &buff2, BUFF_SIZE+8+20, 0, (struct sockaddr*)&server, (socklen_t*)&size_server) == -1) {
            perror("Recv fail");
            close(fd);
            exit(EXIT_FAILURE);
        }

        ip = (struct iphdr*)buff2;
        ip_len = ip->ihl * 4;
        udp = (struct udphdr*)(buff2 + ip_len);
        if(udp->dest == htons(8081)) {
            printf("Received a message: %s\n", buff2 + ip_len + sizeof(*udp));
            break;
        }
    }

    close(fd);
    return 0;
}