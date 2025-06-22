#include <stdio.h>
#include <stdlib.h>
#include <data.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <ncurses.h>
#include <errno.h>

#define MSGLEN 1000

int semid;
volatile int exit_flag = 0;
pthread_mutex_t exit_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ncurses_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
struct data members;
struct data messages;
pid_t pid;
struct message {
    long mtype;
    char mtext[MSGLEN];
    int am_i_read[50];
};

struct for_refresh_thread {
    WINDOW* message;
    WINDOW* message_in;
    WINDOW* members;
    WINDOW* members_in;
};

void* message_handler(void*);
void* refresh_handler(void*);
void send_message(struct message*, char*, int, int);

void sem_sub(int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_post(int semnum) {
    struct sembuf op = {semnum, 1, 0};
    semop(semid, &op, 1);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Use: ./client <nickname>.\n");
        return 0;
    }
    key_t key = ftok(".", 'A');
    struct message* shm;
    int shmid = shmget(key, sizeof(struct message) * 2, 0600);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (void *) -1) {
        perror("shmat");
        exit(1);
    }
    semid = semget(key, 5, 0600);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }
    pid = getpid();
    send_message(&shm[0], argv[1], 1, 1);

    pthread_t mh, rh;
    pthread_create(&mh, NULL, message_handler, (void*)&shmid);

    //ui
    WINDOW* input_box, *input, *chat_box, *chat, *memb_box, *memb;
    
    initscr(); keypad(stdscr, TRUE);
    refresh();
    int cols, rows;
    getmaxyx(stdscr, rows, cols);
    input_box = newwin(3, cols, rows-3, 0);
    input = derwin(input_box, 1, cols-2, 1, 1);
    box(input_box, '|', '-');
    chat_box = newwin(rows-3, cols-12, 0, 0);
    chat = derwin(chat_box, rows-5, cols-14, 1, 1);
    box(chat_box, '|', '-');
    memb_box = newwin(rows-3, 12, 0, cols-12);
    memb = derwin(memb_box, rows-5, 10, 1, 1);
    box(memb_box, '|', '-');
    wrefresh(input_box);
    wrefresh(chat_box);
    wrefresh(memb_box);
    refresh();

    struct for_refresh_thread wins = {
        .message = chat_box,
        .message_in = chat,
        .members = memb_box,
        .members_in = memb
    };
    pthread_create(&rh, NULL, refresh_handler, (void*)&wins);
    char line[256];
    int ch, pos = 0;
    while(1) {
        wrefresh(chat_box);
        wrefresh(chat);
        wrefresh(memb_box);
        wrefresh(memb);
        refresh();
        ch = wgetch(input);
        if(ch == '\n') {
            line[pos] = '\0';
            if(strcmp(line, "!exit") == 0) {
                pthread_mutex_lock(&exit_mutex);
                exit_flag = 1;
                pthread_mutex_unlock(&exit_mutex);
                send_message(&shm[1], &line[0], pid, 3);
                break;
            }
            send_message(&shm[0], &line[0], 2, 3);
            werase(input);
            wmove(input, 0, 0);
            wrefresh(input);
            pos = 0;
            
        } else if(ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if(pos > 0) {
                pos--;
                wmove(input, 0, 2+pos);
                wdelch(input);
                wrefresh(input);
            }
        } else if(pos < 255) {
            line[pos++] = ch;
        }
    }
    delwin(input_box);
    delwin(chat_box);
    delwin(memb_box);
    refresh();
    endwin();

    int* res;
    pthread_join(mh, (void**)res);
    pthread_join(rh, (void**)res);
    send_message(&shm[0], argv[1], 1, 0);
    send_message(&shm[0], argv[1], 2, 4);
    if (shmdt(shm) < 0) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}

void* message_handler(void* mq) {
    struct message msg;
    int* shmid = (int*)mq;
    struct message* shm;
    if ((shm = shmat(*shmid, NULL, 0)) == (void *) -1) {
        perror("shmat");
        exit(1);
    }
    int id = (pid*2654435761)%30;

    data_init(&members);
    data_init(&messages);

    while(!exit_flag) {
        sem_sub(3);
        if(shm[1].am_i_read[id] != 1) {
            shm[1].am_i_read[id] = 1;
            msg.mtype = shm[1].mtype;
            strcpy(msg.mtext, shm[1].mtext);
            pthread_mutex_lock(&data_mutex);
            char* token = strtok(&msg.mtext[0], " ");
            if(strcmp(token, "0") == 0) {
                token = strtok(NULL, " ");
                token = strtok(NULL, " ");
                members.data[data_find(&members, atoi(token))].is_online = 0;
            } else if (strcmp(token, "1") == 0){
                struct data_arr member;
                token = strtok(NULL, " ");
                member.line = malloc(strlen(token)+1);
                strcpy(member.line, token);
                token = strtok(NULL, " ");
                member.id = atoi(token);
                member.is_online = 1;
                data_add(&members, &member);
            } else {
                struct data_arr msg_tmp;
                token = strtok(NULL, ">");
                if(strcmp(token, "!exit") == 0 && shm->mtype == pid) {
                    pthread_mutex_lock(&exit_mutex);
                    exit_flag = 1;
                    pthread_mutex_unlock(&exit_mutex);
                    pthread_mutex_unlock(&data_mutex);
                    break;
                }
                msg_tmp.line = malloc(strlen(token)+1);
                strcpy(msg_tmp.line, token);
                token = strtok(NULL, " ");
                msg_tmp.id = atoi(token);
                data_add(&messages, &msg_tmp);
            }
            pthread_mutex_unlock(&data_mutex);

        } else {
            shm[1].am_i_read[id] = 1;
            usleep(300000);
            continue;
        }
    }
    data_free(&members);
    data_free(&messages);
    if (shmdt(shm) < 0) {
        perror("shmdt");
        exit(1);
    }
    return NULL;
}

void* refresh_handler(void* args) {
    struct for_refresh_thread* wins = (struct for_refresh_thread*) args;
    while(1) {
        pthread_mutex_lock(&exit_mutex);
        if(exit_flag) {
            pthread_mutex_unlock(&exit_mutex);
            break;
        }
        pthread_mutex_unlock(&exit_mutex);
        pthread_mutex_lock(&data_mutex);
        pthread_mutex_lock(&ncurses_mutex);

        werase(wins->members_in);
        werase(wins->message_in);
        for(int i = 0; i < members.size; i++) {
            if(members.data[i].is_online)
                wprintw(wins->members_in, "%s\n", members.data[i].line);
        }
        for(int i = 0; i < messages.size; i++) {
            wprintw(wins->message_in, "<%s>%s\n", members.data[data_find(&members, messages.data[i].id)].line, messages.data[i].line);
        }
        
        wrefresh(wins->message_in);
        wrefresh(wins->members_in);
        wrefresh(wins->message);
        wrefresh(wins->members);
        
        pthread_mutex_unlock(&ncurses_mutex);
        pthread_mutex_unlock(&data_mutex);

        usleep(100000); // 100ms
    }
    return NULL;
}

void send_message(struct message* shm, char* line, int prio, int marker) {
    struct message msg;
    msg.mtype = prio;

    if(marker >= 3) {
        sprintf(&msg.mtext[0], "%d %s>%d", marker, line, pid);
    } else {
        sprintf(&msg.mtext[0], "%d %s %d", marker, line, pid);
    }
    if(prio <= 2){
        sem_sub(2);
        strcpy(shm->mtext, msg.mtext);
        shm->mtype = msg.mtype;
        sem_post(prio-1);
        usleep(100000);
        sem_post(2);
    } else {
        sem_sub(4);
        strcpy(shm->mtext, msg.mtext);
        shm->mtype = msg.mtype;
        for(int i = 0; i < members.size; i++) {
            sem_post(3);
        }
        usleep(100000);
        sem_post(4);
    }
}