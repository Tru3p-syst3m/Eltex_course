#include <stdio.h>
#define N 5000000

int main() {
    int num = 0;

    while(1) {
        num++;
        if(num == N)
            break;
    }

    printf("num: %d\n", num);
    return 0;
}

/*
time ./sequential 
num: 5000000

real    0m0,025s
user    0m0,022s
sys     0m0,003s
*/