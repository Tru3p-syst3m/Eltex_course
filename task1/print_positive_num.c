#include <stdio.h>
// #include <limits.h>
int main(){
    int num, size = 0;
    printf("Type num: ");
    scanf("%d", &num);

    if(num < 0){
        printf("The num must be positive. Retry.\n");
        return 0;
    } else if(num == 0){
        printf("number in binary form: 0\n");
        return 0;
    }

    for (int i = num; i != 0; i /= 2){
        size++;
    }

    char line[size];
    for(int i = 0; i < size; i++){
        char c = 48 + ((num >> i) & 0x1);
        line[size - 1 - i] = c;
    }
    printf("number in binary form: %s\n", line);
    return 0;
}