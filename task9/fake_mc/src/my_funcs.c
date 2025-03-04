#include <dirent.h>
#include <curses.h>
#include <stdlib.h>
#include <my_funcs.h>
#include <errno.h>
#include <path.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <consts.h>
#include <indexes.h>

#define min(a,b) (a < b ? a : b)

int print_dirs(WINDOW* wnd, struct dirent** namelist, struct PATH* path, int ndirs, int target, struct index* ind) {
    wclear(wnd);
    struct stat sb;
    char* filename = malloc(path->size + 30);
    int end = min(ndirs, ind->down);
    for(int i = ind->up; i != end; i++) {
        strcpy(filename, path->path);
        strcat(filename, "/"); strcat(filename, namelist[i]->d_name);
        if(stat(filename, &sb) < 0) {
            fprintf(stderr, "Gettin' file stat: %s\n", strerror(errno));
            free(filename);
            return -1;
        }
        if((sb.st_mode & S_IFMT) == S_IFDIR) {
            wattron(wnd, A_BOLD);
            wattroff(wnd, A_DIM);
        } else {
            wattron(wnd, A_DIM);
            wattroff(wnd, A_BOLD);
        }
        if(i == target)
            wattron(wnd, A_REVERSE);
        wprintw(wnd, "%s\n", namelist[i]->d_name);
        wattroff(wnd, A_REVERSE);
    }
    free(filename);
    return 0;
}

void move_down(WINDOW* wnd, struct dirent** namelist, struct PATH* path, int ndirs, int* target, struct index* ind) {
    *target += (*target == (ndirs-1)) ? 0 : 1;
    refresh_index(ind, *target);
    print_dirs(wnd, namelist, path, ndirs, *target, ind);
    wrefresh(wnd);
}

void move_up(WINDOW* wnd, struct dirent** namelist, struct PATH* path, int ndirs, int* target, struct index* ind) {
    *target -= (*target == 0) ? 0 : 1;
    refresh_index(ind, *target);
    print_dirs(wnd, namelist, path, ndirs, *target, ind);
    wrefresh(wnd);
}

int open_dir(WINDOW* wnd, struct dirent*** namelist, int* ndirs, int* target, struct PATH* path, struct index* ind) {
    struct stat sb;
    char* filename = malloc(path->size + 30);
    strcpy(filename, path->path);
    strcat(filename, "/"); strcat(filename, namelist[0][*target]->d_name);
    if(stat(filename, &sb) < 0) {
        fprintf(stderr, "Gettin' file stat: %s\n", strerror(errno));
        free(filename);
        return -1;
    }
    if((sb.st_mode & S_IFMT) == S_IFDIR) {
        path_add(path, namelist[0][*target]->d_name);
        for(int i = 0; i < *ndirs; i++) {
            free(namelist[0][i]);
        }
        free(*namelist);
        *namelist = NULL;
        *ndirs = scandir(path->path, namelist, 0, alphasort);
        if(*ndirs < 0) {
            fprintf(stderr, "Scanning dirs: %s", strerror(errno));
            path_kill(path);
            free(filename);
            return -1;
        }
        *target = 0;
        refresh_index(ind, *target);
        print_dirs(wnd, *namelist, path, *ndirs, *target, ind);
        wrefresh(wnd);
    }
    free(filename);
    return 0;
}