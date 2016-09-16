#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <stddef.h>

typedef struct allocator allocator_t;

/*  Creates an allocator
    Parameters:
        default_block_size - size of default allocations, this is ignored
                             if an allocation larger that this value is requested.

    Returns:
        initialised allocater.
*/
allocator_t *allocator_init(size_t default_block_size);

/*  Destroy an allocator. */
void allocator_delete(allocator_t *allocator);

/*  Request memory from an allocator
    Parameters:
        allocator - valid allocator from which to request bytes.
        num       - size of the allocation in bytes.

    Returns:
        Pointer to the allocated data.
*/
void *allocator_new(allocator_t *allocator, size_t num);

/*  Push an allocation mark onto the allocator */
void allocator_push(allocator_t *allocator);

/*  Pop an allocation mark off the allocator, deallocating
    any memory allocated since the last allocator_push.
*/
void allocator_pop(allocator_t *allocator);

/* Remove the last mark from the allocator. */
void allocator_forget(allocator_t *allocator);

#endif
