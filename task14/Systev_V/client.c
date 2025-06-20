#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#define SHM_SIZE 1024 

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main() {
    key_t key = ftok(".", 'S');
    int shmid, semid;
    char *shm;
    char* line = "hi!\n";

    if ((shmid = shmget(key, SHM_SIZE, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }
    
    if ((semid = semget(key, 1, 0666)) < 0) {
        perror("semget");
        exit(1);
    }
    
    struct sembuf sb = {0, -1, 0};
    
    if (semop(semid, &sb, 1) < 0) {
        perror("semop");
        exit(1);
    }
    
    printf("Получено из разделяемой памяти: %s", shm);


    strcpy(shm, line);
    
    sb.sem_op = 1;
    if (semop(semid, &sb, 1) < 0) {
        perror("semop");
        exit(1);
    }
    sb.sem_op = -1;
    
    sleep(1);
    
    if (shmdt(shm) < 0) {
        perror("shmdt");
        exit(1);
    }
    
    printf("Клиент завершает работу.\n");
    return 0;
}