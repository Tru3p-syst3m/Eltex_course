#include <stdio.h>
int main(){
    int num;
    printf("Type num: ");
    scanf("%d", &num);

    if(num < 0){
        printf("The num must be positive or smaller. Retry.\n");
        return 0;
    } else if(num == 0){
        printf("Ones in binary form: 0\n");
        return 0;
    }

    int cnt = 0;
    for(int i = 0; i < 32; i++){
        cnt += (num >> i) & 0x1;
    }
    printf("Ones in binary form: %d\n", cnt);
    return 0;
}