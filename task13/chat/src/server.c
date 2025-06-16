#include <stdio.h>
#include <stdlib.h>
#include <data.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#define MSGLEN 1000

pthread_mutex_t members_mutex = PTHREAD_MUTEX_INITIALIZER;

int exit_flag = 0;
struct data members;
struct data messages;

struct message {
    long mtype;
    char mtext[MSGLEN];
};
void shair_database(int, int);
void broadcast(struct message*, int);
void send_underflowing();
void print_members();
void print_messages();
void* members_handler(void*);
void* message_hendler(void*);

int main() {
    key_t key = ftok(".", 'A');
    int mqid = msgget(key, 0600|IPC_CREAT);
    pthread_t m_h;
    pthread_t msg_h;
    pthread_create(&m_h, NULL, members_handler, (void*) &mqid);
    pthread_create(&msg_h, NULL, message_hendler, (void*) &mqid);
    
    int* res;
    pthread_join(m_h, (void**) &res);
    pthread_join(msg_h, (void**) &res);
    msgctl(mqid, IPC_RMID, NULL);
    return 0;
}

void* members_handler(void* mq) {
    struct message msg;
    int mqid = *((int*)mq);
    int member_msg_prio = 1;
    data_init(&members);
    int members_online = members.size;

    while(1) {
        if (msgrcv(mqid, &msg, sizeof(msg.mtext), member_msg_prio, 0) == -1) {
            if (errno == ENOMSG) {
                usleep(100000);
                continue;
            }
            break;
        }
        pthread_mutex_lock(&members_mutex);
        char str_tmp[MSGLEN];
        struct data_arr member;
        strcpy(&str_tmp[0], &msg.mtext[0]); 
        char* token = strtok(&msg.mtext[0], " ");
        if(strcmp(token, "0") == 0) {
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            members.data[data_find(&members, atoi(token))].is_online = 0;
            members_online--;
        } else {
            token = strtok(NULL, " ");
            member.line = malloc(strlen(token)+1);
            strcpy(member.line, token);
            member.is_online = 1;
            token = strtok(NULL, " ");
            member.id = atoi(token);
            shair_database(mqid, member.id);
            data_add(&members, &member);
            members_online++;
        }
        strcpy(&msg.mtext[0], &str_tmp[0]);
        broadcast(&msg, mqid);
        print_members();
        if(members_online <= 0) {
            pthread_mutex_unlock(&members_mutex);
            members.size = 0;
            exit_flag = 1;
            break;
        }
        pthread_mutex_unlock(&members_mutex);
    }
    data_free(&members);
    return NULL;
}

void* message_hendler(void* mq) {
    struct message msg;
    int mqid = *((int*)mq);
    data_init(&messages);

    while(!exit_flag) {
        struct timespec timeout = {.tv_sec = 1, .tv_nsec = 0};
        int rc = msgrcv(mqid, &msg, sizeof(msg.mtext), 2, 0);

        if(rc == -1) {
            if(errno == ENOMSG) {
                nanosleep(&timeout, NULL);
                continue;
            }
            
            if(errno == EINTR) continue;
            
            perror("msgrcv failed");
            continue;
        }
        char text_tmp[MSGLEN];
        strcpy(text_tmp, msg.mtext);
        struct data_arr msg_tmp;
        char* token = strtok(&text_tmp[0], " ");
        if(strcmp(token, "4") == 0) {
            pthread_mutex_lock(&members_mutex);
            if(members.size <= 0) {
                pthread_mutex_unlock(&members_mutex);
                break;
            }
            pthread_mutex_unlock(&members_mutex);
        } else {
            broadcast(&msg, mqid);
            token = strtok(NULL, ">");
            msg_tmp.line = malloc(strlen(token)+1);
            strcpy(msg_tmp.line, token);
            token = strtok(NULL, " ");
            msg_tmp.id = atoi(token);
            data_add(&messages, &msg_tmp);
            print_messages();
        }
        
    }

    data_free(&messages);

    return NULL;
}


void broadcast(struct message* msg, int mqid) {
    for(int i = 0; i < members.size; i++) {
        msg->mtype = (int)members.data[i].id;
        msgsnd(mqid, msg, sizeof(msg->mtext), 0);
    }
}

void shair_database(int mqid, int pid) {
    struct message msg;
    msg.mtype = pid;
    char text[MSGLEN];
    for(int i = 0; i < members.size; i++) {
        if(members.data[i].is_online) {
            sprintf(&text[0], "1 %s %d", members.data[i].line, members.data[i].id);
            strcpy(&msg.mtext[0], &text[0]);
            msgsnd(mqid, &msg, sizeof(msg.mtext), 0);
        }
    }
    for(int i = 0; i < messages.size; i++) {
        sprintf(&text[0], "3 %s>%d", messages.data[i].line, messages.data[i].id);
        strcpy(&msg.mtext[0], &text[0]);
        msgsnd(mqid, &msg, sizeof(msg.mtext), 0);
    }
}


void print_members(){
    printf("---------------------------------------------\n");
    printf("members %d\n", members.size);
    if(members.size == 0)
        printf("all member gone\n");
    for(int i = 0; i < members.size; i++) {
        printf("member %s, %d\n", members.data[i].line, members.data[i].id);
    }
}

void print_messages(){
    printf("---------------------------------------------\n");
    printf("messages %d\n", messages.size);
    if(messages.size == 0)
        printf("no messages\n");
    for(int i = 0; i < messages.size; i++) {
        printf("%s, %d\n", messages.data[i].line, messages.data[i].id);
    }
}