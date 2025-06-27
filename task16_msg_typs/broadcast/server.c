#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BUFF_SIZE 16
#define SOCKET_PORT 8080
#define BC_ADDR "255.255.255.255"
int main() { 
    //Socet preparation
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(SOCKET_PORT);
    client.sin_addr.s_addr = inet_addr(BC_ADDR);
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        perror("Socet fail");
        exit(EXIT_FAILURE);
    }
    int flag = 1; 
    if(setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag)) < 0) {
        perror("setsockopt fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    //Buff preparation
    char buff[BUFF_SIZE];
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    sprintf(buff, "%02d:%02d:%02d\n", 
            timeinfo->tm_hour, 
            timeinfo->tm_min, 
            timeinfo->tm_sec);

    //Sanding
    socklen_t size_client = sizeof(client);
    if(sendto(fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&client, size_client) < 0) {
        perror("sand fail");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);
    return 0;
}