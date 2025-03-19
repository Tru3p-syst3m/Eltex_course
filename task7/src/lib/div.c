#include <stdio.h>
#include <div.h>
#include <clear_buff.h>

int divide() {
    int a,b;
    printf("Введите первый аргумент: ");
    scanf("%d", &a);
    clear_buff();
    printf("Введите второй аргумент: ");
    scanf("%d", &b);
    clear_buff();
    printf("%d / %d = %.1f\n", a, b, (double)a/(double)b);
    return 0;
}