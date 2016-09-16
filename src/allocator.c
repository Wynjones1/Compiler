#include "allocator.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct region region_t;

/* A region is a block of memory used by the allocator.*/
struct region
{
    size_t    size;   // Total number of bytes in the region.
    size_t    used;   // Number of bytes used in the region.
    region_t *prev;   // Previous region.
    char      data[]; // Data allocated for the region.
};

typedef struct mark
{
    region_t *region;
    size_t    used;
}mark_t;

typedef struct allocator
{
    size_t     default_size;
    mark_t    *marks;
    size_t     num_marks;
    region_t  *current;
}allocator_t;

static region_t *make_region(size_t size, region_t *prev)
{
    region_t *out = malloc(sizeof(region_t) + size);
    out->size  = size;
    out->used  = 0;
    out->prev  = prev;
    return out;
}

allocator_t *allocator_init(size_t default_size)
{
    allocator_t *out = malloc(sizeof(allocator_t));
    out->default_size = default_size;
    out->current      = make_region(default_size, NULL);
    out->marks        = NULL;
    out->num_marks    = 0;
    allocator_push(out);
    return out;
}

void *allocator_new(allocator_t *al, size_t num)
{
    region_t *r = al->current;
    // If we have space in the current region use it.
    if(r->size - r->used > num)
    {
        void * out = r->data + r->used;
        r->used += num;
        return out;
    }

    // Create a new region at least as big as the allocation request.
    size_t s = al->default_size;
    if(s < num)
    {
        s = num;
    }
    al->current = make_region(s, r);
    al->current->used = num;
    return al->current->data;
}

void allocator_push(allocator_t *al)
{
    size_t num = al->num_marks;
    al->marks = realloc(al->marks, sizeof(mark_t) * (num + 1));
    al->marks[num].region = al->current;
    al->marks[num].used   = al->current->used;
    al->num_marks += 1;
}

void allocator_pop(allocator_t *al)
{
    assert(al->num_marks > 0);
    mark_t *mark = al->marks + al->num_marks - 1;
    region_t *r = al->current;
    while(r != NULL && r != mark->region)
    {
        region_t *prev = r->prev;
        free(r);
        r = prev;
    }
    if(r != NULL)
    {
        r->used = mark->used;
    }
    al->num_marks -= 1;
}

void allocator_delete(allocator_t *al)
{
    region_t *r = al->current;
    while(r != NULL)
    {
        region_t *prev = r->prev;
        free(r);
        r = prev;
    }
    if(al->marks)
    {
        free(al->marks);
    }
    free(al);
}

void allocator_forget(allocator_t *al)
{
    al->num_marks -= 1;
}
