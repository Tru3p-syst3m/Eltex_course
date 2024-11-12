#include <stdio.h>

int main(){
    //getting length of array
    int len;
    while(1){
        printf("Array length: ");
        scanf("%d", &len);
        if(len < 1){
            printf("\nTry again.\n");
            continue;
        } else {
            break;
        }
        printf("\n");
    }

    //filling & reversing the array 
    int arr[len];
    for(int i = 0; i < len; i++)
        arr[i] = i+1;
    for(int i = 0; i < len; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
    for(int i = 0; i < len - i; i++){
        int buff = arr[i];
        arr[i] = arr[len - i - 1];
        arr[len - i - 1] = buff;
    }

    //printing array
    for(int i = 0; i < len; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
    return 0;
}