#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <clear_buff.h>
#include <add.h>
#include <sub.h>
#include <mul.h>
#include <div.h>

int main() {
    system("clear");
    char c;
    while(c != '5') {
        printf("Выберите действие:\n");
        printf("1) Сложение\n");
        printf("2) Вычитание\n");
        printf("3) Умножение\n");
        printf("4) Деление\n");
        printf("5) Выход\n");
        printf("Команда: ");
        c = getchar();
        clear_buff();

        switch (c)
        {
        case '1':
            add();
            break;
        
        case '2':
            sub();
            break;
        
        case '3':
            mult();
            break;

        case '4':
            divide();
            break;
        
        case '5':
            break;
        
        default:
            printf("Нет такой комнды. Попробуй еще раз.\n");
            sleep(2);
            break;
        }

    }
    system("clear");
    return 0;
}