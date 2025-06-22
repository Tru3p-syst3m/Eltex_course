#pragma once
#include <sys/types.h>

struct data {
  int size;
  int capacity;
  int is_message;
  struct data_arr* data;
};

struct data_arr
{
    char* line;
    pid_t id;
    int is_online;
};

void data_init(struct data*);
void data_free(struct data*);
void data_add(struct data*, struct data_arr*);
void data_delete(struct data*, pid_t);
int data_find(struct data*, pid_t);
char* data_get_line(struct data*, pid_t);