#include <stdio.h>
int main(){
    int num, byte;
    printf("Type num to change: ");
    scanf("%d", &num);
    if(num < 0){
        printf("The num must be positive or smaller. Retry.\n");
        return 0;
    }
    printf("Type num to input: ");
    scanf("%d", &byte);
    if(byte < 0 || byte > 0xff){
        printf("The num must be in range from 0 to 255. Retry.\n");
        return 0;
    }
    byte = byte << 16;
    num = num & 0xff00ffff;
    num = num | byte;
    printf("The retulting num: dec %d\n", num);
}