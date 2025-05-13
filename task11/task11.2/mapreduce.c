#include <stdio.h>
#include <pthread.h>

#define N 5000000
#define T 4

void* thread_work(void* args) {
    int* num = (int*) args;
    *num = 0;
    
    while(1) {
        *num = *num + 1;
        if(*num == N/T)
            break;
    }
    return NULL;
}

int main() {
    pthread_t threads[T];
    int results[T];
    int num = 0;
    for(int i = 0; i < T; i++) {
        pthread_create(&threads[i], NULL, thread_work, (void*) &results[i]);
    }
    for(int i = 0; i < T; i++) {
        pthread_join(threads[i], NULL);
        num += results[i];
    }

    printf("num: %d\n", num);
    return 0;
}

/*
time ./mapreduce 
num: 5000000

real    0m0,011s
user    0m0,023s
sys     0m0,005s
*/