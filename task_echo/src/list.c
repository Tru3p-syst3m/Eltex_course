#include <list.h>
#include <stdlib.h>

struct list *list_init() {
    struct list* l = malloc(sizeof(struct list));
    l->size = 0;
    l->head = NULL;
    l->tail = NULL;
    return l;
}

struct node* list_find(struct list* list, int ip, int port) {
    struct node* n = list->head;
    while(n != NULL) {
        if(n->ip == ip && n->port == port) {
            break;
        }
        n = n->next;
    }
    return n;
}

int list_add(struct list* list, int ip, int port) {
    if(list->head == NULL) {
        struct node* n = malloc(sizeof(struct node));
        n->ip = ip;
        n->port = port;
        n->next = NULL;
        n->count = 1;
        list->head = n;
        list->tail = n;
        list->size++;
        return n->count;
    }

    struct node* n = list_find(list, ip, port);
    if(n == NULL) {
        n = malloc(sizeof(struct node));
        n->ip = ip;
        n->port = port;
        n->next = NULL;
        n->count = 1;
        list->tail->next = n;
        list->tail = n;
        list->size++;
        return n->count;
    }
    n->count++;
    return n->count;
}

void list_switch_off(struct list* list, int ip, int port) {
    struct node* n = list_find(list, ip, port);
    if(n == NULL) {
        return;
    }
    n->count = 0;
}

void list_kill(struct list* list) {
    struct node* n = list->head;
    struct node* pn;
    while(n != NULL) {
        pn = n;
        n = n->next;
        free(pn);
    }
}
