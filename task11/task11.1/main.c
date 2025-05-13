#include <stdio.h>
#include <pthread.h>

#define N 5

void* print_index(void* args) {
    int* index = (int*) args;
    printf("thread with index: %d\n", *index);
    return NULL;
}

int main() {
    pthread_t threads[N];
    int buff[N];
    int* res;
    for(int i = 0; i < N; i++) {
        buff[i] = i;
        pthread_create(&threads[i], NULL, print_index, (void*) &buff[i]);
    }
    for(int i = 0; i < N; i++) {
        pthread_join(threads[i], (void**) &res);
    }
    return 0;
}