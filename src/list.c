#include "list.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct list
{
    size_t count;
    size_t size;
    char *data;
}list_t;

list_t *list_init(size_t size)
{
    list_t *out = malloc(sizeof(list_t));
    out->count = 0;
    out->size = size;
    out->data = NULL;
    return out;
}

void list_push(list_t *l, void *data)
{
    l->data = realloc(l->data, l->size * (l->count + 1));
    memcpy(l->data + (l->count * l->size), data, l->size);
    l->count++;
}

void list_delete(list_t *l)
{
    if(l->data != NULL)
    {
        free(l->data);
    }
    free(l);
}

size_t list_count(list_t*l)
{
    return l->count;
}

void *list_data(list_t*l)
{
    return l->data;
}
