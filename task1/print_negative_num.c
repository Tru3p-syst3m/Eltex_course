#include <stdio.h>
int main(){
    int num;
    printf("Type num: ");
    scanf("%d", &num);

    if(num > 0){
        printf("The num must be negative or bigger. Retry.\n");
        return 0;
    } else if(num == 0){
        printf("number in binary form: 0\n");
        return 0;
    }

    char line[32];
    for(int i = 0; i < 32; i++){
        char c = 48 + ((num >> i) & 0x1);
        line[31 - i] = c;
    }
    printf("number in binary form: %s\n", line);
    return 0;
}