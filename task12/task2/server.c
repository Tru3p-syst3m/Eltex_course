#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
int main() {
    char* fifo_name = "pipe";
    char* message = "Hi!\n";
    int len = 5;
    if(mkfifo(fifo_name, 0600) == -1) {
        perror("mkfifo() error");
    }
    int fd = open(fifo_name, O_WRONLY);
    if (write(fd, message, len) == -1) {
        perror("write() error");
    }
    close(fd);
    return 0;
}