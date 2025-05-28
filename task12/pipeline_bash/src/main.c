#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <malloc.h>
#define MAX_COMMAND_SIZE 60
#define MAX_ARGS 16

typedef struct Command {
    char* path;
    char** args;
} command;

command* parse(char*, int*);
void command_handler(command*, int, int);
void pipeline_handler(command*, int*);

int main() {
    char* line = malloc(sizeof(char)* MAX_COMMAND_SIZE);

    while(1) {
        printf("> ");
        if (!fgets(line, MAX_COMMAND_SIZE, stdin)) {
            if (feof(stdin)) break;
            perror("fgets failed");
            continue;
        }
        if(strcmp(line, "exit\n") == 0) {
            break;
        }
        int count = 0;
        command* commands = parse(line, &count);
        if (!commands && count > 0) {
            fprintf(stderr, "parse failed\n");
            continue;
        }
        if(count == 1) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                exit(EXIT_FAILURE);
            } else if(pid == 0) {
                command_handler(commands, STDIN_FILENO, STDOUT_FILENO);
            }
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid failed");
            }
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Command exited with status %d\n", WEXITSTATUS(status));
            }
        } else {
            pipeline_handler(commands, &count);
        }

        //clean_up
        for (int i = 0; i < count; i++) {
            free(commands[i].path);
            for (int j = 0; commands[i].args[j] != NULL; j++) {
                free(commands[i].args[j]);
            }
            free(commands[i].args);
        }
        free(commands);
    }
    free(line);
    
    return 0;
}

command* parse(char* str, int* count) {
    str[strcspn(str, "\n")] = '\0';
    
    int commands_size = 1;
    command* commands = malloc(sizeof(command) * commands_size);
    if (!commands) {
        perror("malloc failed");
        return NULL;
    }
    *count = 0;
    
    char* saveptr;
    char* com = strtok_r(str, "|", &saveptr);
    
    while (com != NULL) {
        while (*com == ' ') com++;
        char* end = com + strlen(com) - 1;
        while (end > com && *end == ' ') end--;
        *(end + 1) = '\0';
        
        if (strlen(com) == 0) {
            com = strtok_r(NULL, "|", &saveptr);
            continue;
        }
        
        char** args = malloc(sizeof(char*) * MAX_ARGS);
        if (!args) {
            perror("malloc failed");
            return NULL;
        }
        int arg_count = 0;
        
        char* arg_saveptr;
        char* arg = strtok_r(com, " ", &arg_saveptr);
        while (arg != NULL) {
            args[arg_count] = strdup(arg);
            if (!args[arg_count]) {
                perror("strdup failed");
                return NULL;
            }
            arg_count++;
            if (arg_count >= MAX_ARGS-1) break;
            arg = strtok_r(NULL, " ", &arg_saveptr);
        }
        args[arg_count] = NULL;
        
        commands[*count].path = strdup(args[0]);
        if (!commands[*count].path) {
            perror("strdup failed");
            return NULL;
        }
        commands[*count].args = args;
        (*count)++;
        
        if (*count >= commands_size) {
            command* tmp = realloc(commands, sizeof(command) * (*count + 1));
            if (!tmp) {
                perror("realloc failed");
                return NULL;
            }
            commands = tmp;
            commands_size++;
        }
        
        com = strtok_r(NULL, "|", &saveptr);
    }
    
    return commands;
}

void command_handler(command* com, int in, int out) {
    if (in != STDIN_FILENO) {
        if (dup2(in, STDIN_FILENO) == -1) {
            perror("dup2 stdin failed");
            exit(EXIT_FAILURE);
        }
        close(in);
    }
    if (out != STDOUT_FILENO) {
        if (dup2(out, STDOUT_FILENO) == -1) {
            perror("dup2 stdout failed");
            exit(EXIT_FAILURE);
        }
        close(out);
    }
    if (com) {
        execvp(com->path, com->args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
}

void pipeline_handler(command* commands, int* count) {
    pid_t pids[*count];
    int pipes[*count-1][2];
    for (int i = 0; i < *count-1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < *count; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
                close(pipes[i-1][0]);
            }
            if (i < *count-1) {
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
            }
            
            for (int j = 0; j < *count-1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            execvp(commands[i].path, commands[i].args);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        
        if (i > 0) {
            close(pipes[i-1][0]);
        }
        if (i < *count-1) {
            close(pipes[i][1]);
        }
        
        pids[i] = pid;
    }

    for (int i = 0; i < *count-1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < *count; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) == -1) {
            perror("waitpid failed");
        }
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Command %d exited with status %d\n", i, WEXITSTATUS(status));
        }
    }
}