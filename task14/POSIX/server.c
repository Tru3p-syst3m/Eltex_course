#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>

#define SHM_NAME "/demo_shm"
#define SEM_NAME "/demo_sem"
#define SHM_SIZE 1024

int main() {
    int shm_fd;
    char *shm_ptr;
    sem_t *sem;
    char* line = "hello!\n";

    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен. Ожидание данных...\n");

    strcpy(shm_ptr, line);

    if (sem_post(sem) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }

    sleep(10);

    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
    
    printf("Получено из разделяемой памяти: %s", shm_ptr);

    // Закрываем и удаляем ресурсы
    if (sem_close(sem) == -1) {
        perror("sem_close");
    }

    if (sem_unlink(SEM_NAME) == -1) {
        perror("sem_unlink");
    }

    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        perror("munmap");
    }

    if (close(shm_fd) == -1) {
        perror("close");
    }

    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }

    printf("Сервер завершает работу.\n");
    return EXIT_SUCCESS;
}