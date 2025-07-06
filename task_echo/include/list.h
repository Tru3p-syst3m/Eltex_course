#include <netinet/ip.h>

struct node {
  int ip;
  int port;
  int count;
  struct node *next;
};

struct list {
  struct node *head;
  struct node *tail;
  int size;
};

struct list* list_init();
struct node* list_find(struct list*, int, int);
int list_add(struct list*, int, int);
void list_switch_off(struct list*, int, int);
void list_kill(struct list*);