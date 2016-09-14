#ifndef OPERATORS_H
#define OPERATORS_H
#include "lexer.h"

enum ACCOCIATIVITY
{
    LEFT,
    RIGHT,
};

#define X_OPERATOR_LIST \
    X(ADD, 4, LEFT) \
    X(SUB, 4, LEFT) \
    X(MUL, 3, LEFT) \
    X(DIV, 3, LEFT) \
    X(MOD, 3, LEFT) \
    X(LT , 6, LEFT) \
    X(LTE, 6, LEFT) \
    X(GT , 6, LEFT) \
    X(GTE, 6, LEFT) \
    X(EQ , 7, LEFT) \
    X(NEQ, 7, LEFT)

enum OPERATOR
{
#define X(NAME, PREC, ASSOC) OPERATOR_ ## NAME = TOKEN_TYPE_OP_ ## NAME,
    X_OPERATOR_LIST
#undef X
};

int is_operator(enum TOKEN_TYPE type);

#endif
