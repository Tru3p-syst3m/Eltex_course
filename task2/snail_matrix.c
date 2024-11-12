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
    int top = 0, buttom = matrix_size - 1;
    int left = 0, right = matrix_size - 1;
    while(top <= buttom && left <= right){
        for(int i = left; i <= right; i++){
            matrix[top][i] = cnt++;
        }
        top++;

        for(int i = top; i <= buttom; i++){
            matrix[i][right] = cnt++;
        }
        right--;

        for(int i = right; i >= left; i--){
            matrix[buttom][i] = cnt++;
        }
        buttom--;

        for(int i = buttom; i >= top; i--){
            matrix[i][left] = cnt++;
        }
        left++;
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