#ifndef HELPERS_H
#define HELPERS_H
#include <assert.h>

#define NOT_IMPLEMENTED() assert(0);

#define TODO_ERROR_HANDLING(...)                \
    do                                          \
    {                                           \
        fprintf(stderr, ##__VA_ARGS__); exit(-1);\
    }                                           \
    while(0)

#endif