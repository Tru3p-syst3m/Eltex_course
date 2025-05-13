#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define CUSTOMER_NUM 3
#define MIN(a,b) (((a)<(b))?(a):(b))

bool is_loader_working = 1;
int storedge[5] = {500, 500, 500, 500, 500};
pthread_mutex_t cell0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cell1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cell2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cell3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cell4 = PTHREAD_MUTEX_INITIALIZER;

void print_storedge(int move) {
    for(int i = 0; i < 5; i++) {
        if(i == move) {
            printf("\033[31m[%d]\033[0m", storedge[i]);
            continue;
        }
        printf("[%d]", storedge[i]);
    }
    printf("\n###########################################\n");
}

void get_items(int choice, pthread_mutex_t* cell, int* need) {
    pthread_mutex_lock(cell);
    int min_val = MIN(storedge[choice], *need);
    storedge[choice] -= min_val;
    *need -= min_val;
    printf("---Customer %d get %d from the shelf. Current need: %d\n", getpid(), min_val, *need);
    print_storedge(choice);
    pthread_mutex_unlock(cell);
}

void set_item(pthread_mutex_t* cell, int choice) {
    pthread_mutex_lock(cell);
    storedge[choice] += 500;
    printf("***Loader set a packedge to the cell %d.\n", choice);
    print_storedge(choice);
    pthread_mutex_unlock(cell);
}

void* customer_routine(void* args) {
    int need = (10 + (rand() % 3)) * 1000;

    while(need != 0) {
        int choice = rand() % 5;
        switch (choice)
        {
        case 0:
            get_items(choice, &cell0, &need);
            break;
        
        case 1:
            get_items(choice, &cell1, &need);
            break;

        case 2:
            get_items(choice, &cell2, &need);
            break;

        case 3:
            get_items(choice, &cell3, &need);
            break;

        case 4:
            get_items(choice, &cell4, &need);
            break;
        default:
            break;
        }
        sleep(2);
    }

    return NULL;
}

void* loader_routine(void* args) {
    while(is_loader_working) {
        int choice = rand() % 5;
        switch (choice)
        {
        case 0:
            set_item(&cell0, choice);
            break;
        
        case 1:
            set_item(&cell1, choice);
            break;

        case 2:
            set_item(&cell2, choice);
            break;

        case 3:
            set_item(&cell3, choice);
            break;

        case 4:
            set_item(&cell4, choice);
            break;
        default:
            break;
        }
        sleep(1);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t customers[CUSTOMER_NUM];
    pthread_t loader;

    for(int i = 0; i < CUSTOMER_NUM; i++) {
        pthread_create(&customers[i], NULL, customer_routine, NULL);
    }
    pthread_create(&loader, NULL, loader_routine, NULL);

    for(int i = 0; i < CUSTOMER_NUM; i++) {
        pthread_join(customers[i], NULL);
    }
    is_loader_working = 0;
    pthread_join(loader, NULL);
}