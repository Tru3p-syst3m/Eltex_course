#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    char* message = "Hi!\n";
    int len = 5;
    int pid;
    int fd[2];
    pipe(fd);
    pid = fork();
    if(pid == -1) {
        perror("fork() error");
    } else if(pid == 0) {
        char str4print[len];
        close(fd[1]);
        if(read(fd[0], str4print, len) == -1) {
            perror("read() error");
        }
        printf("%s", str4print);
        close(fd[0]);
        return 0;
    } else {
        close(fd[0]);
        if(write(fd[1], message, len) == -1) {
            perror("write() error");
        }
    }
    close(fd[1]);
    waitpid(pid, &fd[0], 0);
    return 0;
}