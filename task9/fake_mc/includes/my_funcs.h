#pragma once
#include <dirent.h>
#include <curses.h>
#include <path.h>
#include <indexes.h>

int print_dirs(WINDOW*, struct dirent**, struct PATH*, int, int, struct index*);
void move_up(WINDOW*, struct dirent**, struct PATH*, int, int*, struct index*);
void move_down(WINDOW*, struct dirent**, struct PATH*, int, int*, struct index*);
int open_dir(WINDOW*, struct dirent***, int* , int*, struct PATH*, struct index*);