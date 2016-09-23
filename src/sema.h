#ifndef SEMA_H
#define SEMA_H

typedef struct allocator allocator_t;
typedef struct sema_state sema_state_t;
typedef struct ast ast_t;

sema_state_t *sema_state_init(allocator_t *alloc);
ast_t *sema(ast_t *ast, sema_state_t *state);

#endif