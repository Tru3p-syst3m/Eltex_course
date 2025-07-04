#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define BUFF_SIZE 7
#define SOCKET_PORT 8080
#define DEST_IP "192.168.3.2"
#define SEND_IP "192.168.3.4"

#pragma pack(push, 1)
typedef struct {
    uint8_t  dest_mac[6];
    uint8_t  src_mac[6];
    uint16_t ether_type;
    struct iphdr ip;
    struct udphdr udp;
    char data[BUFF_SIZE];
} EthernetPacket;
#pragma pack(pop)

uint16_t check_sum(uint16_t *ptr, int count) {
  long sum = 0;
  uint16_t byte;

  while (count > 1) {
    sum += *ptr++;
    count -= 2;
  }
  if (count == 1) {
    byte = 0;
    *((u_char *)&byte) = *(u_char *)ptr;
    sum += byte;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum = sum + (sum >> 16);

  return (uint16_t)~sum;
}

int main() {
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock == -1) {
        perror("socket() failed");
        return EXIT_FAILURE;
    }

    const char *iface_name = "wlp0s20f3";                    //INTERFACE!!!!
    int ifindex = if_nametoindex(iface_name);
    if (ifindex == 0) {
        close(sock);
        return EXIT_FAILURE;
    }

    EthernetPacket packet;

    uint8_t dest_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};       // MAC!!!!!!!!!!!!!
    uint8_t src_mac[6] = {0x20, 0xC1, 0x9B, 0x0C, 0x6C, 0x46};      // MAC!!!!!!!!!!!!!

    memcpy(packet.dest_mac, dest_mac, 6);
    memcpy(packet.src_mac, src_mac, 6);
    packet.ether_type = htons(ETH_P_IP);

    packet.ip.version  = 4;
    packet.ip.ihl      = 5;
    packet.ip.tos      = 0;
    packet.ip.tot_len  = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + BUFF_SIZE);
    packet.ip.id       = htons(12345);
    packet.ip.frag_off = 0;
    packet.ip.ttl      = 64;
    packet.ip.protocol = IPPROTO_UDP;
    packet.ip.check    = 0;
    packet.ip.saddr    = inet_addr(SEND_IP);            // IP!!!!!!!!!!!
    packet.ip.daddr    = inet_addr(DEST_IP);            // IP!!!!!!!!!!!!!!

    packet.udp.source = htons(8081);
    packet.udp.dest   = htons(SOCKET_PORT);
    packet.udp.len    = htons(sizeof(struct udphdr) + BUFF_SIZE);
    packet.udp.check  = 0;

    strcpy(packet.data, "Hello!");

    packet.ip.check = check_sum((uint16_t *)&packet.ip, sizeof(struct iphdr));

    struct sockaddr_ll sa = {0};
    sa.sll_family   = AF_PACKET;
    sa.sll_protocol = htons(ETH_P_IP);
    sa.sll_ifindex  = ifindex;
    sa.sll_halen    = ETH_ALEN;
    memcpy(sa.sll_addr, packet.dest_mac, ETH_ALEN);

    printf("Sending: %s\n", packet.data);
    ssize_t sent = sendto(sock, &packet, sizeof(packet), 0, (struct sockaddr*)&sa, sizeof(sa));

    if (sent == -1) {
        perror("sendto() failed");
        close(sock);
        return EXIT_FAILURE;
    }

    printf("Sent %zd bytes\n", sent);

    printf("Waiting for reply...\n");
    char recv_buf[1500];
    while (1) {
        ssize_t recv_len = recvfrom(sock, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
        if (recv_len == -1) {
            perror("recvfrom() failed");
            break;
        }

        EthernetPacket *recv_pkt = (EthernetPacket*)recv_buf;
        if (ntohs(recv_pkt->udp.dest) == 8081) {
            printf("Received: %s\n", recv_pkt->data);
            break;
        }
    }

    close(sock);
    return 0;
}