#include <gtest/gtest.h>

extern "C" {
    #include "parser.h"
}

ast_t *pf(const char *data, ast_t *(*func)(parse_state_t*))
{
    token_list_t *tl = tokenise(data);
    parse_state_t *ps = parse_state_new(tl);
    return func(ps);
}

TEST(parser, function_0) {
    const char *data = "function a(){}";
    ast *out = pf(data, parse_function);
    ASSERT_EQ(out->type, AST_TYPE_FUNCTION);
}

TEST(parser, function_1) {
    const char *data =
    "function a() -> int"
    "{}";
    ast *out = pf(data, parse_function);
    ASSERT_EQ(out->type, AST_TYPE_FUNCTION);
}

TEST(parser, function_2) {
    const char *data =
    "function a(int : b) -> int"
    "{}";
    ast *out = pf(data, parse_function);
    ASSERT_EQ(out->type, AST_TYPE_FUNCTION);
}

TEST(parser, function_3) {
    const char *data =
    "function a(int : b) -> int"
    "{ return 0; }";
    ast *out = pf(data, parse_function);
    ASSERT_EQ(out->type, AST_TYPE_FUNCTION);
}

TEST(parser, func_call_0) {
    const char *data = "f()";
    ast *out = pf(data, parse_func_call);
    ASSERT_EQ(out->type, AST_TYPE_FUNC_CALL);
}

TEST(parser, func_call_1) {
    const char *data = "func(1, 2, 3)";
    ast *out = pf(data, parse_func_call);
    ASSERT_EQ(out->type, AST_TYPE_FUNC_CALL);
}

TEST(parser, func_call_2) {
    const char *data = "func()()";
    ast *out = pf(data, parse_func_call);
    ASSERT_EQ(out->type, AST_TYPE_FUNC_CALL);
}

TEST(parser, if_0) {
    const char *data = "if(0){}";
    ast *out = pf(data, parse_if);
    ASSERT_EQ(out->type, AST_TYPE_IF);
}

TEST(parser, if_else_0) {
    const char *data = "if(0){}else{}";
    ast *out = pf(data, parse_if);
    ASSERT_EQ(out->type, AST_TYPE_IF);
}
