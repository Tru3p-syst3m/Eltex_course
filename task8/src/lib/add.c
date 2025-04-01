#include <stdio.h>
#include <add.h>
#include <clear_buff.h>

int add() {
    int a,b;
    printf("Введите первый аргумент: ");
    scanf("%d", &a);
    clear_buff();
    printf("Введите второй аргумент: ");
    scanf("%d", &b);
    clear_buff();
    printf("%d + %d = %d\n", a, b, a+b);
    return 0;
}