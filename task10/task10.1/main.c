#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid = fork();

    if(pid == -1) {
        perror("Fork faild");
    } else if(pid == 0) {
        printf("Это дочерний процесс его pid: %d, pid родителя: %d.\n", getpid(), getppid());
        return 255;
    } else {
        printf("Это родительский процесс его pid: %d. pid дочернего процесса: %d\n", getpid(), pid);
    }

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Дочерний процесс завершился с кодом %d\n", WEXITSTATUS(status));
    }

    return 0;
}