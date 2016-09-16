#ifndef OPERATORS_H
#define OPERATORS_H
#include "lexer.h"

enum ACCOCIATIVITY
{
    LEFT,
    RIGHT,
};

#define X_OPERATOR_LIST \
    X(ADD, 4, 2, LEFT) \
    X(SUB, 4, 2, LEFT) \
    X(MUL, 3, 2, LEFT) \
    X(DIV, 3, 2, LEFT) \
    X(MOD, 3, 2, LEFT) \
    X(LT , 6, 2, LEFT) \
    X(LTE, 6, 2, LEFT) \
    X(GT , 6, 2, LEFT) \
    X(GTE, 6, 2, LEFT) \
    X(EQ , 7, 2, LEFT) \
    X(NEQ, 7, 2, LEFT)

enum OPERATOR
{
#define X(NAME, PREC, NUM_ARGS, ASSOC) OPERATOR_ ## NAME = TOKEN_TYPE_OP_ ## NAME,
    X_OPERATOR_LIST
#undef X
};

int is_operator(enum TOKEN_TYPE type);

#endif
