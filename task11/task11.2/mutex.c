#include <stdio.h>
#include <pthread.h>

#define N 5000000
#define T 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_work(void* args) {
    int* num = (int *) args;
    while(1) {
        pthread_mutex_lock(&mutex);
        if(*num >= N) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        *num = *num + 1;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t threads[T];
    int num = 0;

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
time ./mutex 
num: 5000000

real    0m34,364s
user    0m2,626s
sys     0m10,541s
*/