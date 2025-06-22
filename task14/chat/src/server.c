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
#define MSGLEN 1000

pthread_mutex_t members_mutex = PTHREAD_MUTEX_INITIALIZER;

int semid;
int members_online = 0;
int exit_flag = 0;
struct data members;
struct data messages;

struct message {
    long mtype;
    char mtext[MSGLEN];
    int am_i_read[50];
};
int checksum(int*);
void shair_database(struct message*, int);
void broadcast(struct message*, struct message*, int);
void send_underflowing();
void print_members();
void print_messages();
void* members_handler(void*);
void* message_hendler(void*);

void sem_sub(int semnum) {
    struct sembuf op = {semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_post(int semnum) {
    struct sembuf op = {semnum, 1, 0};
    semop(semid, &op, 1);
}

void fill(int* arr, int num) {
    for(int i = 0; i < 50; i++) {
        arr[i] = num;
    }
}


int main() {
    key_t key = ftok(".", 'A');
    int shmid = shmget(key, sizeof(struct message) * 2, 0600|IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }
    semid = semget(key, 5, 0600|IPC_CREAT);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }
    
    semctl(semid, 0, SETVAL, 0); // member server
    semctl(semid, 1, SETVAL, 0); // message server
    semctl(semid, 2, SETVAL, 1); // write client
    semctl(semid, 3, SETVAL, 0); // message/member client
    semctl(semid, 4, SETVAL, 1); // write server
    pthread_t m_h;
    pthread_t msg_h;
    int args[2] = {shmid, semid};
    pthread_create(&m_h, NULL, members_handler, (void*) args);
    pthread_create(&msg_h, NULL, message_hendler, (void*) args);
    printf("Server is running...\n");
    int* res;
    pthread_join(m_h, (void**) &res);
    pthread_join(msg_h, (void**) &res);
    if (semctl(semid, 0, IPC_RMID, 0) < 0) {
        perror("semctl");
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        perror("shmctl");
        exit(1);
    }
    return 0;
}

void* members_handler(void* mq) {
    int shmid = *((int*)mq);
    struct message* shm;
    if ((shm = shmat(shmid, NULL, 0)) == (void *) -1) {
        perror("shmat");
        exit(1);
    }
    struct message msg;
    int pid_tmp = 0;
    data_init(&members);
    members_online = members.size;

    while(1) {
        sem_sub(0);
        if (shm[0].mtype != 1) {
            usleep(100000);
            continue;
        }
        printf("<members> got message: %s\n", shm[0].mtext);
        msg.mtype = shm[0].mtype;
        strcpy(msg.mtext, shm[0].mtext);
        pthread_mutex_lock(&members_mutex);
        char str_tmp[MSGLEN];
        struct data_arr member;
        strcpy(&str_tmp[0], &msg.mtext[0]); 
        char* token = strtok(&msg.mtext[0], " ");
        if(strcmp(token, "0") == 0) {
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            pid_tmp = atoi(token);
            members.data[data_find(&members, pid_tmp)].is_online = 0;
            members_online--;
        } else {
            token = strtok(NULL, " ");
            member.line = malloc(strlen(token)+1);
            strcpy(member.line, token);
            member.is_online = 1;
            members_online++;
            token = strtok(NULL, " ");
            member.id = atoi(token);
            pid_tmp = member.id;
            shair_database(&shm[1], member.id);                         //
            data_add(&members, &member);
        }
        strcpy(&msg.mtext[0], &str_tmp[0]);
        broadcast(&msg, &shm[1], 0);                                       // 
        print_members();
        if(members_online <= 0) {
            pthread_mutex_unlock(&members_mutex);
            members.size = 0;
            exit_flag = 1;
            break;
        }
        pthread_mutex_unlock(&members_mutex);
    }
    if (shmdt(shm) < 0) {
        perror("shmdt");
        exit(1);
    }
    data_free(&members);
    return NULL;
}

void* message_hendler(void* mq) {
    int shmid = *((int*)mq);
    struct message* shm;
    if ((shm = shmat(shmid, NULL, 0)) == (void *) -1) {
        perror("shmat");
        exit(1);
    }
    struct message msg;
    data_init(&messages);
    while(!exit_flag) {
        sem_sub(1);
        if (shm[0].mtype != 2) {
            usleep(100000);
            continue;
        }
        printf("<messages> got message: %s\n", shm[0].mtext);
        msg.mtype = shm[0].mtype;
        strcpy(msg.mtext, shm[0].mtext);
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
            broadcast(&msg, &shm[1], 0);                              //
            token = strtok(NULL, ">");
            msg_tmp.line = malloc(strlen(token)+1);
            strcpy(msg_tmp.line, token);
            token = strtok(NULL, " ");
            msg_tmp.id = atoi(token);
            data_add(&messages, &msg_tmp);
            print_messages();
        }
        
    }
    if (shmdt(shm) < 0) {
        perror("shmdt");
        exit(1);
    }
    data_free(&messages);

    return NULL;
}


void broadcast(struct message* msg, struct message* shm, int pid) {
    sem_sub(4);
    fill(shm->am_i_read, 0);
    if(pid != 0) {
        pid *= 2654435761;
        shm->am_i_read[pid%30] = 1;
    }

    strcpy(shm->mtext, msg->mtext);
    printf("%s", msg->mtext);
    shm->mtype = 0;
    
    while(checksum(shm->am_i_read) != members_online) {
        for(int i = 0; i < members_online; i++) {
            sem_post(3);
            printf("broadcast\n");
            usleep(300000);
        }
    }
    sem_post(4);
}

void shair_database(struct message* shm, int pid) {
    sem_sub(4);
    int id = (pid*2654435761)%30;
    fill(shm->am_i_read, 1);
    shm->am_i_read[id] = 0;
    shm->mtype = 0;
    char text[MSGLEN];
    for(int i = 0; i < members.size; i++) {
        if(members.data[i].is_online) {
            sprintf(&text[0], "1 %s %d", members.data[i].line, members.data[i].id);
            strcpy(&shm->mtext[0], &text[0]);
            while(shm->am_i_read[id] != 1)
            {
                for(int i = 0; i < members_online*2; i++) {
                    sem_post(3);
                }
                usleep(300000);
            }
            fill(shm->am_i_read, 1);
            shm->am_i_read[id] = 0;
        }
    }
    for(int i = 0; i < messages.size; i++) {
        sprintf(&text[0], "3 %s>%d", messages.data[i].line, messages.data[i].id);
        strcpy(&shm->mtext[0], &text[0]);
        while(shm->am_i_read[id] != 1) {
            for(int i = 0; i < members_online*2; i++) {
                sem_post(3);
            }
            usleep(300000);
        }
        fill(shm->am_i_read, 1);
        shm->am_i_read[id] = 0;
    }
    sem_post(4);
}

int checksum(int* arr) {
    int count = 0; 
    for(int i = 0; i < 50; i++) {
        count += arr[i];
    }
    return count;
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