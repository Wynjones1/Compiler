#include <gtest/gtest.h>

extern "C" {
    #include "lexer.h"
}

TEST(lexer, empty) {
    const char *data = "";

    token_list_t *tl = tokenise(data);
    ASSERT_EQ(tl_size(tl), 1);
    ASSERT_EQ(tl_get(tl, 0)->type, TOKEN_TYPE_EOF);
}

TEST(lexer, lparen) {
    const char *data = "(";

    token_list_t *tl = tokenise(data);
    ASSERT_EQ(tl_size(tl), 2);
    ASSERT_EQ(tl_get(tl, 0)->type, TOKEN_TYPE_LPAREN);
    ASSERT_EQ(tl_get(tl, 1)->type, TOKEN_TYPE_EOF);
}

TEST(lexer, keyword_function) {
    const char *data = "function";

    token_list_t *tl = tokenise(data);
    ASSERT_EQ(tl_size(tl), 2);
    ASSERT_EQ(tl_get(tl, 0)->type, TOKEN_TYPE_KW_FUNCTION);
    ASSERT_EQ(tl_get(tl, 1)->type, TOKEN_TYPE_EOF);
}
