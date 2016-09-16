cmake_minimum_required(VERSION 2.8)

set(SRC
    allocator.c
    ast.c
    eval.c
    lexer.c
    operators.c
    parser.c
    string_common.c
    list.c
)

file(GLOB HDR *.h)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-return-type -Wno-unused-parameter -Wno-unused-variable")

add_library(compilerlib SHARED ${SRC} ${HDR})

add_executable(compiler main.c)
target_link_libraries(compiler compilerlib)

add_executable(lexer lexer_main.c)
target_link_libraries(lexer compilerlib)

add_executable(parser parser_main.c)
target_link_libraries(parser compilerlib)

add_executable(eval eval_main.c)
target_link_libraries(eval compilerlib)

add_executable(parser_test parser_test_main.c)
target_link_libraries(parser_test compilerlib ${CMAKE_DL_LIBS})
