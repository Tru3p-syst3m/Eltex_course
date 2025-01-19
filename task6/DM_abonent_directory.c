#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <DM_abonent_directory.h>

int directory_init(struct Directory* dir) {
    dir->capacity = 0;
    dir->head = NULL;
    dir->tail = NULL;
    return 0;
}

struct abonent* abonent_crate() {
    struct abonent* abonent = malloc(sizeof(struct abonent));
    if(!abonent){
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);        
    }
    for(int i = 0; i < DATA_SIZE; i++) {
        abonent->name[i] = '\n';
        abonent->second_name[i] = '\n';
        abonent->tel[i] = '\n';
    }
    return abonent;
}

int cell_add(struct Directory* dir, struct abonent* abonent) {
    if(dir->capacity == 100) {
        fprintf(stderr, "directory overflow\n");
        exit(1);
    }
    struct Cell* cell = malloc(sizeof(struct Cell));
    if(!cell){
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);        
    }
    if(!dir->head){
        dir->head = cell;
        dir->tail = cell;
        cell->prev = NULL;
        cell->next = NULL;
        cell->data = abonent;
        dir->capacity++;
        return 0;
    }
    cell->prev = dir->tail;
    cell->next = NULL;
    cell->data = abonent;

    dir->tail->next = cell;
    dir->tail = cell;

    dir->capacity++;
    return 0;
}

struct Cell* cell_search(struct Directory* dir, char* name) {
    struct Cell* cur = dir->head;
    for(int i = 0; i < dir->capacity && cur; i++) {
        if(!strcmp(cur->data->name, name)){
            return cur;
        }
        cur = cur->next;
    }

    return cur;
}

int dir_free(struct Directory* dir) {
    struct Cell* cur = dir->head;
    for(int i = 0; i < dir->capacity && cur; i++) {
        struct Cell* buff = cur->next;
        free(cur->data);
        free(cur);
        cur = buff;
    }
    dir->capacity = 0;
    dir->head = NULL;
    dir->tail = NULL;

    return 0;
}

int cell_remove(struct Directory* dir, struct Cell* cell) {
    if(!cell)
        return 0;
    if(dir->head == cell) {
        if(cell->next){
            cell->next->prev = NULL;
        }
        dir->head = cell->next;
        dir->capacity--;
        free(cell->data);
        free(cell);
        return 0;
    } else if(dir->tail == cell) {
        if(cell->prev){
            cell->prev->next = NULL;
        }
        dir->tail = cell->prev;
        dir->capacity--;
        free(cell->data);
        free(cell);
        return 0;
    } else {
        cell->next->prev = cell->prev;
        cell->prev->next = cell->next;
        dir->capacity--;
        free(cell->data);
        free(cell);
    }
    return 0;
}

void abonent_print(struct abonent* ab) {
    printf("\nИмя: %s\nФамилия: %s\nНомер: %s\n\n", ab->name, ab->second_name, ab->tel);
}