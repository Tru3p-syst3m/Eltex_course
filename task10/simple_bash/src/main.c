#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

void run_child(char **args) {
    // Игнорируем SIGTTOU на время смены группы
    signal(SIGTTOU, SIG_IGN);

    // Создаём новую группу и забираем терминал
    setpgid(0, 0);
    if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1) {
        perror("tcsetpgrp(child) failed");
        exit(1);
    }
    signal(SIGTTOU, SIG_DFL);  // Восстанавливаем обработчик

    // Запускаем программу
    execvp(args[0], args);
    perror("execvp failed");
    exit(1);
}

char **parse_command(char *input) {
    int buffer_size = 64;
    int position = 0;
    char **tokens = malloc(buffer_size * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "malloc faild\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(input, " \t\r\n\a");  // Разделители: пробел, табуляция, перевод строки и т.д.
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buffer_size) {
            buffer_size *= 2;
            tokens = realloc(tokens, buffer_size * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "realloc faild\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;  // execvp требует NULL в конце
    return tokens;
}

int main() {
    char input[100];
    char **args;
    while(1) {
        fgets(input, sizeof(input), stdin);
        if(strcmp(input, "exit\n") == 0)
            break;

        args = parse_command(input);

        int pid = fork();
        if(pid == -1) {
            perror("fork faild");
        } else if(pid == 0) {
            run_child(args);
        }

        int status;
        waitpid(pid, &status, WUNTRACED);
        signal(SIGTTOU, SIG_IGN);
        if (tcsetpgrp(STDIN_FILENO, getpgid(0)) == -1) {
            perror("tcsetpgrp (parent) failed");
        }
        signal(SIGTTOU, SIG_DFL);
        free(args);
    }

    return 0;
}