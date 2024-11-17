#include <stdio.h>

int main() {
    int num;
    int byte;
    printf("Type a num: ");
    scanf("%d", &num);
    printf("Type a byte: ");
    while(1) {
        scanf("%d", &byte);
        if(byte < 0 || byte > 0xff){
            printf("byte is a num from 0 to 255\n");
            continue;
        } else {
            break;
        }
    }
    char* ptr = (char*)&num;
    ptr += 2;
    *ptr = byte;
    printf("%d\n", num);
    return 0;
}