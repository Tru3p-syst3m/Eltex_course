#include <data.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

void data_init(struct data* m) {
    m->capacity = 1;
    m->size = 0;
    m->data = malloc(sizeof(struct data_arr));
    if(!m->data) {
        perror("ERROR: data_init() malloc error");
        exit(1);
    }
}

void data_free(struct data* m) {
    free(m->data);
}

void data_add(struct data* m, struct data_arr* data_arr) {
    if(m->size == m->capacity) {

        m->capacity *= 2;
        m->data = realloc(m->data, sizeof(struct data_arr) * m->capacity);
        if(!m->data) {
            perror("ERROR: data_add() realloc error");
            exit(1);
        }
    }
    m->data[m->size].id = data_arr->id;
    m->data[m->size].is_online = data_arr->is_online;
    m->data[m->size].line = data_arr->line;
    m->size++;
}

void data_delete(struct data* m, pid_t id) {
    int index = data_find(m, id);
    for(; index < m->size-1; index++) {
        m->data[index] = m->data[index+1];
    }
    m->size--;
}

int data_find(struct data* m, pid_t id) {
    for(int i = 0; i < m->size; i++) {
        if(m->data[i].id == id) {
            return i;
        }
    }
    return -1;
}

char* data_get_line(struct data* m, pid_t id){
    int index = data_find(m, id);
    return m->data[index].line;
}
