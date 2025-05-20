#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main() {
    char* fifo_name = "pipe";
    int len = 5;
    char message[len];
    int fd = open(fifo_name, O_RDONLY);
    if (read(fd, message, len) == -1) {
        perror("read() error");
    }
    printf("%s", message);
    close(fd);
    return 0;
}