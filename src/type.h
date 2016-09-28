#ifndef TYPE_H
#define TYPE_H

typedef struct type type_t;

const char *base_types[] =
{
    "int",
};

struct type
{
    const char *name;
};

#endif