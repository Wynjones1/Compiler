#include "operators.h"

int is_operator(enum TOKEN_TYPE type)
{
#define X(NAME, PREC, ASSOC) if(type == TOKEN_TYPE_OP_ ## NAME){return 1;}
    X_OPERATOR_LIST
#undef X
    return 0;
}

