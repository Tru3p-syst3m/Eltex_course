#include <stdio.h>
#include <pthread.h>

#define N 5000000
#define T 4

pthread_spinlock_t spinlock;

void* thread_work(void* args) {
    int* num = (int *) args;
    while(1) {
        pthread_spin_lock(&spinlock);
        if(*num == N){
            pthread_spin_unlock(&spinlock);
            break;
        }
        *num = *num + 1;
        printf("thread with index: %d\n", *num);
        pthread_spin_unlock(&spinlock);
    }

    return NULL;
}

int main() {
    pthread_t threads[T];
    int num = 0;

    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
    for(int i = 0; i < T; i++) {
        pthread_create(&threads[i], NULL, thread_work, (void*) &num);
    }

    for(int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("num: %d\n", num);

    return 0;
}

/*
time ./spinlock 
num: 5000000

real    0m42,109s
user    2m0,980s
sys     0m6,548s
*/