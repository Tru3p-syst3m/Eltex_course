#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

#define BUFF_SIZE 16
#define SOCKET_PORT_UDP 8080
#define SOCKET_PORT_TCP 8081

int main() {
    //UDP socet preparation
    struct sockaddr_in server_udp;
    memset(&server_udp, 0, sizeof(server_udp));
    server_udp.sin_family = AF_INET;
    server_udp.sin_port = htons(SOCKET_PORT_UDP);
    server_udp.sin_addr.s_addr = INADDR_ANY;
    socklen_t size_server_udp = sizeof(server_udp), size_client_udp;
    int fd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd_udp == -1) {
        perror("UDP socet fail\n");
        exit(EXIT_FAILURE);
    }
    if(bind(fd_udp, (struct sockaddr*)&server_udp, size_server_udp) == -1) {
        perror("UDP bind fail\n");
        close(fd_udp);
        exit(EXIT_FAILURE);
    }

    //TCP socet preparation
    struct sockaddr_in server_tcp;
    memset(&server_tcp, 0, sizeof(server_tcp));
    server_tcp.sin_family = AF_INET;
    server_tcp.sin_port = htons(SOCKET_PORT_TCP);
    server_tcp.sin_addr.s_addr = INADDR_ANY;
    socklen_t size_server_tcp = sizeof(server_tcp), size_client_tcp;
    int fd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_tcp == -1) {
        perror("TCP socet fail\n");
        exit(EXIT_FAILURE);
    }
    if(bind(fd_tcp, (struct sockaddr*)&server_tcp, size_server_tcp) == -1) {
        perror("TCP bind fail\n");
        close(fd_udp);
        close(fd_tcp);
        exit(EXIT_FAILURE);
    }
    listen(fd_tcp, 3);

    //Main cycle
    printf("Server is working...\n");
    printf("Listening...\n");
    fd_set fds;
    char buff[BUFF_SIZE];
    struct timeval tv;
    time_t rawtime;
    struct tm *timeinfo;
    while(1){
        FD_ZERO(&fds);
        FD_SET(fd_udp, &fds);
        FD_SET(fd_tcp, &fds);
        int max_fd = (fd_udp < fd_tcp) ? fd_tcp : fd_udp;

        tv.tv_sec = 0;
        tv.tv_usec = 500000; 

        int ready = select(max_fd+1, &fds, NULL, NULL, &tv);
        if(ready == -1) {
            perror("Select fail\n");
            close(fd_udp);
            close(fd_tcp);
            exit(EXIT_FAILURE);
        }
        if(ready != 0) {
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            //UDP socet handler
            if(FD_ISSET(fd_udp, &fds)) {
                struct sockaddr_in client;
                size_client_udp = sizeof(client);
                memset(&client, 0, size_client_udp);
                if(recvfrom(fd_udp, &buff, BUFF_SIZE, 0, (struct sockaddr*)&client, &size_client_udp) == -1) {
                    perror("UDP recv fail\n");
                    close(fd_udp);
                    close(fd_tcp);
                    exit(EXIT_FAILURE);
                }
                if(strcmp(buff, "kill") == 0) {
                    printf("Received a kill message from UDP segment\n");
                    break;
                }
                
                sprintf(buff, "%02d:%02d:%02d\n", 
                        timeinfo->tm_hour, 
                        timeinfo->tm_min, 
                        timeinfo->tm_sec);

                if(sendto(fd_udp, buff, BUFF_SIZE, 0, (struct sockaddr*)&client, size_client_udp) == -1) {
                    perror("UDP send fail\n");
                    close(fd_udp);
                    close(fd_tcp);
                    exit(EXIT_FAILURE);
                }
            }

            //TCP socet handler
            if(FD_ISSET(fd_tcp, &fds)) {
                struct sockaddr_in client;
                size_client_tcp = sizeof(client);
                memset(&client, 0, size_client_tcp);

                int client_fd = accept(fd_tcp, (struct sockaddr*)&client, &size_client_tcp);
                /*
                    тут можно было бы добавлять дискрипторы клиентов в массив и множество,
                    а после этого ифа проверять их на готовность чтения и записи,
                    но в задаче нет продолжительного соединения. так что я этого не делал:)
                */
                if(client_fd == -1) {
                    perror("TCP accept fail\n");
                    close(fd_udp);
                    close(fd_tcp);
                    exit(EXIT_FAILURE);
                }
                if(recv(client_fd, &buff, BUFF_SIZE, 0) == -1) {
                    perror("TCP recv fail\n");
                    close(fd_udp);
                    close(fd_tcp);
                    exit(EXIT_FAILURE);
                }
                if(strcmp(buff, "kill") == 0) {
                    printf("Received a kill message from TCP segment\n");
                    break;
                }

                sprintf(buff, "%02d:%02d:%02d\n", 
                        timeinfo->tm_hour, 
                        timeinfo->tm_min, 
                        timeinfo->tm_sec);

                if(send(client_fd, &buff, BUFF_SIZE, 0) == -1) {
                    perror("TCP send fail");
                    close(fd_udp);
                    close(fd_tcp);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    close(fd_udp);
    close(fd_tcp);
    return 0;
}