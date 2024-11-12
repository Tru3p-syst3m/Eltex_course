#include <stdio.h>

int main(){
    //getting size of matrix
    int matrix_size;
    while(1){
        printf("Matrix size: ");
        scanf("%d", &matrix_size);
        if(matrix_size < 1){
            printf("\nTry again.\n");
            continue;
        } else {
            break;
        }
        printf("\n");
    }

    //filling the matrix
    int matrix[matrix_size][matrix_size], cnt = 1;
    for(int i = 0; i < matrix_size; i++){
        for(int j = 0; j < matrix_size; j++){
            matrix[i][j] = cnt++;
        }
    }

    //printing the matrix
    for(int i = 0; i < matrix_size; i++){
        for(int j = 0; j < matrix_size; j++){
            printf("%4d", matrix[i][j]);
        }
        printf("\n");
    }
    return 0;
}