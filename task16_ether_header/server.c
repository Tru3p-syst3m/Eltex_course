// #include <stdio.h>
// #include <arpa/inet.h>
// #include <errno.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>

// #define BUFF_SIZE 7
// #define SOCKET_PORT 8080
// #define IP "192.168.3.2"

// int main() {
//     struct sockaddr_in server, client;
//     memset(&server, 0, sizeof(server));
//     memset(&client, 0, sizeof(client));
//     server.sin_family = AF_INET;
//     server.sin_port = htons(SOCKET_PORT);
//     server.sin_addr.s_addr = inet_addr(IP);
//     socklen_t size_server = sizeof(server), size_client;
    
//     char buff[BUFF_SIZE];
    
//     int fd = socket(AF_INET, SOCK_DGRAM, 0);
//     if(fd == -1) {
//         perror("Socet fail");
//         exit(EXIT_FAILURE);
//     }

//     if(bind(fd, (struct sockaddr*)&server, size_server) == -1) {
//         perror("Bind fail");
//         close(fd);
//         exit(EXIT_FAILURE);
//     }

//     printf("Server is working...\n");

//     size_client = sizeof(client);
//     printf("Listening...\n");
//     if(recvfrom(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)&client, &size_client) == -1) {
//         perror("Recv fail");
//         close(fd);
//         exit(EXIT_FAILURE);
//     }

//     printf("Server received a message: %s\n", buff);
//     buff[0] = '!';

//     printf("Server sending a message: %s\n", buff);
//     if(sendto(fd, &buff, BUFF_SIZE, 0, (struct sockaddr*)&client, size_client) == -1) {
//         perror("Send fail");
//         close(fd);
//         exit(EXIT_FAILURE);
//     }

//     close(fd);
//     return 0;
// }

#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define BUFF_SIZE 7
#define SOCKET_PORT 8080
#define IP "192.168.3.2"

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    SOCKET fd;
    struct sockaddr_in server, client;
    int size_client = sizeof(client);
    char buff[BUFF_SIZE];

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SOCKET_PORT);
    server.sin_addr.s_addr = inet_addr(IP);

    if(bind(fd, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(fd);
        WSACleanup();
        return 1;
    }

    printf("Server is working...\n");
    printf("Listening...\n");

    if(recvfrom(fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&client, &size_client) == SOCKET_ERROR) {
        printf("Receive failed\n");
        closesocket(fd);
        WSACleanup();
        return 1;
    }

    printf("Server received a message: %s\n", buff);
    buff[0] = '!';

    printf("Server sending a message: %s\n", buff);
    if(sendto(fd, buff, BUFF_SIZE, 0, (struct sockaddr*)&client, size_client) == SOCKET_ERROR) {
        printf("Send failed\n");
        closesocket(fd);
        WSACleanup();
        return 1;
    }

    closesocket(fd);
    WSACleanup();
    return 0;
}