#include <stdio.h>
#include <ctype.h>
#include <string.h>

struct abonent {
    char name[10];
    char second_name[10];
    char tel[10];
};

struct Directory {
    struct abonent directory[100];
    int capacity;
};

void clear_data(char* data) {
    for(int i = 0; i < 10; i++) {
        data[i] = 0;
    }
}

void clear_abonent(struct abonent* ab) {
    clear_data(ab->name);
    clear_data(ab->second_name);
    clear_data(ab->tel);
}

void copy_data(char* data, char* place) {
    strcpy(place, data);
}

void directory_init(struct Directory* dir) {
    dir->capacity = 0;
    for(int i = 0; i < 100; i++) {
        clear_abonent(&dir->directory[i]);
    }
}

int find_abonent_index(char* data, struct Directory* dir, int index) {
    int i = index;
    for(; i < 100; i++) {
        if(!strcmp(data, dir->directory[i].name))
            break;
    }
    if (i >= 100) {
        return -1;
    }

    return i;
}

void print_abonent(struct abonent* ab) {
    printf("Имя: %s\nФамилия: %s\nНомер: %s\n\n", ab->name, ab->second_name, ab->tel);
}

void clear_input_buffer() {
    while (getchar() != '\n');
}

int main() {
    struct Directory dir;
    directory_init(&dir);
    char data[10];
    int index;
    while(1){
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

            clear_data(data);
            printf("Введите имя(не более 9-ти симовлов): ");
            scanf("%9s", data);
            data[9] = '\0';
            printf("data: %s\n", data);
            copy_data(data, dir.directory[dir.capacity].name);
            clear_input_buffer();
            
            clear_data(data);
            printf("Введите фамилию(не более 9-ти симовлов): ");
            scanf("%9s", data);
            data[9] = '\0';
            copy_data(data, dir.directory[dir.capacity].second_name);
            clear_input_buffer();

            clear_data(data);
            printf("Введите номер(не более 9-ти цифр): ");
            scanf("%9s", data);
            data[9] = '\0';
            copy_data(data, dir.directory[dir.capacity].tel);
            clear_input_buffer();

            dir.capacity++;
            break;
        case 2:
            if(dir.capacity == 0) {
                printf("В справочнике не зарегистрированно обанентов.\n");
                break;
            }
            printf("Введите имя удаляемого абонента: ");
            scanf("%9s", data);
            data[9] = '\0';
            clear_input_buffer();
            index = find_abonent_index(data, &dir, 0);
            if(index < 0) {
                printf("Такого абонента нет.\n");
                break;
            }
            clear_abonent(&dir.directory[index]);
            dir.capacity--;
            for(;index < dir.capacity; index++) {
                copy_data(dir.directory[index+1].name, dir.directory[index].name);
                copy_data(dir.directory[index+1].second_name, dir.directory[index].second_name);
                copy_data(dir.directory[index+1].tel, dir.directory[index].tel);
            }
            clear_abonent(&dir.directory[index]);
            break;
        case 3:
            clear_data(data);
            printf("Введите имя абонента: ");
            scanf("%9s", data);
            data[9] = '\0';
            clear_input_buffer();
            index = -1;
            while(1){
                int buff = index;
                index = find_abonent_index(data, &dir, index + 1);
                if(index > buff) {
                    print_abonent(&dir.directory[index]);
                    continue;
                } else if(buff > index) {
                    break;
                } else if(buff == index) {
                    printf("Aбонентов с таким именем нет.\n");
                    break;
                } 
            }
            break;
        case 4:
            for(int i = 0; i < dir.capacity; i++) {
                print_abonent(&dir.directory[i]);
            }
            break;
        case 5:
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