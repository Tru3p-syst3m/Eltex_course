#define DATA_SIZE 11

struct abonent {
  char name[DATA_SIZE];
  char second_name[DATA_SIZE];
  char tel[DATA_SIZE];
};

struct Cell {
  struct abonent* data;
  struct Cell *prev;
  struct Cell *next;
};

struct Directory {
  struct Cell *head;
  struct Cell *tail;
  int capacity;
};

int directory_init(struct Directory*);

struct abonent* abonent_crate();
void abonent_print(struct abonent*);
int cell_add(struct Directory*, struct abonent*);
struct Cell* cell_search(struct Directory*, char*);
int cell_remove(struct Directory*, struct Cell*);
int dir_free(struct Directory*);