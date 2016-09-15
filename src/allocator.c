#include "allocator.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct region region_t;

/* A region is a block of memory used by the allocator.*/
struct region
{
    size_t    size;   // Total number of bytes in the region.
    size_t    used;   // Number of bytes used in the region.
    bool      start;  // Indicates if this is the start of the region.
    region_t *prev;   // Previous region.
    char      data[]; // Data allocated for the region.
};

typedef struct allocator
{
    size_t default_size;
    region_t *current;
}allocator_t;

static region_t *make_region(size_t size, bool start, region_t *prev)
{
    region_t *out = malloc(sizeof(region_t) + size);
    out->size  = size;
    out->used  = 0;
    out->start = start;
    out->prev  = prev;
    return out;
}

allocator_t *allocator_init(size_t default_size)
{
    allocator_t *out = malloc(sizeof(allocator_t));
    out->default_size = default_size;
    out->current = NULL;
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
    al->current = make_region(s, false, r);
    al->current->used = num;
    return al->current->data;
}

void allocator_push(allocator_t *al)
{
    al->current = make_region(al->default_size, true, al->current);
}

void allocator_pop(allocator_t *al)
{
    region_t *r = al->current;
    bool done = false;
    while(!done && r != NULL)
    {
        // If we are at the start of the region we stop freeing
        if(r->start)
        {
            done = true;
        }
        region_t *prev = r->prev;
        free(r);
        r = prev;
    }
    al->current = r;
}

void allocate_delete(allocator_t *al)
{
    region_t *r = al->current;
    while(r != NULL)
    {
        region_t *prev = r->prev;
        free(r);
        r = prev;
    }
    free(al);
}
