#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <DM_abonent_directory.h>

void clear_input_buffer() {
    while (getchar() != '\n');
}

int main() {
    struct Directory dir; 
    directory_init(&dir);
    
    while(1) {
        printf("1) Добавить абонента\n2) Удалить абонента\n3) Поиск абонентов по имени\
           \n4) Вывод всех записей\n5) Выход\nВведите цифру: ");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
            case 1:
                if(dir.capacity == 100){
                    printf("Справочник полон.\n");
                    break;
                }
                struct abonent* abonent = abonent_crate();

                printf("Введите имя(не более 10-ти симовлов): ");
                scanf("%10s", abonent->name);
                clear_input_buffer();
                printf("Введите фамилию(не более 10-ти симовлов): ");
                scanf("%10s", abonent->second_name);
                clear_input_buffer();
                printf("Введите номер(не более 10-ти цифр): ");
                scanf("%10s", abonent->tel);
                clear_input_buffer();

                cell_add(&dir, abonent);
                break;
            case 2:
                if(dir.capacity == 0){
                    printf("В справочнике не зарегистрированно обанентов.\n");
                    break;
                }
                char name_d[DATA_SIZE];
                printf("Введите имя удаляемого абонента: ");
                scanf("%10s", name_d);
                clear_input_buffer();
                cell_remove(&dir, cell_search(&dir, name_d));
                break;
            case 3:
                if(dir.capacity == 0){
                    printf("В справочнике не зарегистрированно обанентов.\n");
                    break;
                }
                char name_s[DATA_SIZE];
                printf("Введите имя абонента: ");
                scanf("%10s", name_s);
                clear_input_buffer();

                struct Cell* cell = cell_search(&dir, name_s);
                if(!cell){
                    printf("\nТакого абонента нет в справочнике\n");
                } else {
                    abonent_print(cell->data);
                }
                break;
            case 4:
                struct Cell* cur = dir.head;
                while(cur) {
                    abonent_print(cur->data);
                    cur = cur->next;
                }
                break;
            case 5:
                dir_free(&dir);
                return 0;
                break;
            default:
                printf("Неправильная команда. Попробуйте снова.\n");
                clear_input_buffer();
                break;
        }
    }
    return 0;
}