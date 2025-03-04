#pragma once

struct PATH {
  char *path;
  int size;
  int capacity;
};

int path_init(struct PATH *, const char *);
int path_add(struct PATH*, char*);
int path_kill(struct PATH*);
