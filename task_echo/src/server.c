#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <signal.h>
#include <list.h>

#define BUFF_SIZE 1028
#define SERVER_PORT 8080

struct list* list;

void kill_handler(int signum, siginfo_t *info, void *context) {
    list_kill(list);
    pause();
}

void* command_handler(void* args) {
    pid_t* pid = (pid_t*) args;
    char str[20];
    while(1) {
        fgets(str, 20, stdin);
        if(strcmp(str, "kill\n") == 0) {
            kill(*pid, SIGUSR1);
            usleep(100000);
            kill(*pid, SIGTERM);
            break;
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in client;
    char buff[BUFF_SIZE];
    
    struct sigaction sa;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sa.sa_mask = set;
    sa.sa_sigaction = kill_handler;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    pthread_t thread;
    pid_t pid = getpid();
    pthread_create(&thread, NULL, command_handler, (void*)&pid);
    pthread_detach(thread);

    list = list_init();

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if(fd == -1) {
        perror("Socket fail");
        exit(EXIT_FAILURE);
    }

    while(1) {
        socklen_t client_size = sizeof(client);
        if(recvfrom(fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&client, &client_size) == -1){
            perror("Recv fail");
            close(fd);
            exit(EXIT_FAILURE);
        }

        struct iphdr* iph = (struct iphdr*)buff;
        int iph_size = iph->ihl * 4;
        struct udphdr* udph = (struct udphdr*)(buff + iph_size);
        char* payload = buff + iph_size + sizeof(struct udphdr);
        
        if(ntohs(udph->dest) == SERVER_PORT) {
            if(strcmp(payload, "!exit\n") == 0) {
                list_switch_off(list, iph->saddr, udph->source);
                continue;
            }

            char count[1];
            *count = list_add(list, iph->saddr, udph->source) + '0';
            
            char response[BUFF_SIZE - iph_size - sizeof(struct udphdr)];
            payload[strlen(payload)-1] = '\0';
            snprintf(response, sizeof(response), "%s %c", payload, *count);

            struct udphdr reply_udph;
            reply_udph.source = htons(SERVER_PORT);
            reply_udph.dest = udph->source;
            reply_udph.len = htons(sizeof(struct udphdr) + strlen(response) + 1);
            reply_udph.check = 0;
            
            memcpy(buff + iph_size, &reply_udph, sizeof(reply_udph));
            memcpy(buff + iph_size + sizeof(reply_udph), response, strlen(response) + 1);
            
            if(sendto(fd, buff + iph_size, sizeof(reply_udph) + strlen(response) + 1, 
                0, (struct sockaddr*)&client, client_size) == -1) {
                perror("Send fail");
                close(fd);
                exit(EXIT_FAILURE);
            }
        }
    }

    close(fd);
    return 0;
}