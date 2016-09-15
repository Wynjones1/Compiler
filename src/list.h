#ifndef LIST_H
#define LIST_H
#include <stddef.h>

typedef struct list list_t;

list_t *list_init(size_t size);
void list_push(list_t *l, void *data);
void list_delete(list_t *l);
size_t list_count(list_t*l);
void *list_data(list_t*l);
#endif
