#include <path.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int path_init(struct PATH* path, const char* start) {
    path->size = strlen(start);
    path->capacity = path->size + 100;
    path->path = malloc(path->capacity);
    if(!path->path){
        return -1;
    }
    strcpy(path->path, start);
    return 0;
}

int path_add(struct PATH* path, char* new_part) {
    if(path->size + sizeof(new_part) >= path->capacity) {
        path->path = realloc(path->path, path->capacity + 100);
        if(!path->path){
            return -1;
        }
    }
    if(path->path[path->size-1] != '/')
        strcat(path->path, "/");
    strcat(path->path, new_part);
    if(strcmp(new_part, "..") == 0) {
        char* new_path = realpath(path->path, NULL);
        free(path->path);
        path->path = new_path;
    }
    path->size += strlen(path->path);
    return 0;
}

int path_kill(struct PATH* path) {
    free(path->path);
    return 0;
}