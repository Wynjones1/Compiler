#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <stddef.h>

typedef struct allocator allocator_t;
void        *allocator_new(allocator_t *allocator, size_t num);
void         allocator_push(allocator_t *allocator);
void         allocator_pop(allocator_t *allocator);
allocator_t *allocator_init(size_t default_block_size);

#endif
