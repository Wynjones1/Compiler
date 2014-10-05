#pragma once
#include <assert.h>

#define MALLOC_T(type)            ((type*) malloc(sizeof(type)))
#define CALLOC_T(num, type)       ((type*) calloc(num, sizeof(type)))
#define REALLOC_T(ptr, type, num) ((type*) realloc(ptr, sizeof(type) * num))
