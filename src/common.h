#pragma once
#include <assert.h>
#include <stdbool.h>

#define MALLOC_T(type)            ((type*) malloc(sizeof(type)))
#define MALLOC_ARRAY_T(type, num) ((type*) malloc(sizeof(type) * (num)));
#define CALLOC_T(num, type)       ((type*) calloc((num), sizeof(type)))
#define REALLOC_T(ptr, type, num) ((type*) realloc(ptr, sizeof(type) * (num)))
