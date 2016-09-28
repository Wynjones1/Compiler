#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

typedef struct intermediate_gen_state intermediate_gen_state_t;
typedef struct ast                    ast_t;
typedef struct intermediate           intermediate_t;

intermediate_gen_state_t *intermediate_state_init();
intermediate_t *intermediate_gen(ast_t *ast, intermediate_gen_state_t *state);
#endif