#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 65536

// Функция для вывода IP-адреса
void print_ip(uint32_t ip) {
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    printf("snifed dns pack from %d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);
}

int main() {
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    char buff[BUFFER_SIZE];
    printf("listening...\n");
    while(1) {
        int msglen = recv(fd, buff, BUFFER_SIZE, 0);
        if(msglen < 0) {
            close(fd);
            perror("recv");
            exit(EXIT_FAILURE);
        }

        struct iphdr* ip = (struct iphdr*) buff;
        if(ip->protocol != IPPROTO_UDP) continue;

        unsigned int ip_len = ip->ihl * 4;

        struct udphdr* udp = (struct udphdr*) (buff + ip_len);
        if (ntohs(udp->source) != 53) continue;
        
        print_ip(ntohl(ip->saddr));
    }
}