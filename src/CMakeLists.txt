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
	sema.c
    symbol_table.c
    intermediate.c
    type.c
)

file(GLOB HDR *.h)

#add_c_flags_if_available(-Wall -Wextra -Werror)
add_c_flags_if_available(-Wno-return-type -Wno-unused-parameter -Wno-unused-variable)

add_c_flags_if_available(/wd4715 /wd4716 /wd4201 /wd4710 /wd4820 /wd4100)

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_library(compilerlib STATIC ${SRC} ${HDR})

add_executable(compiler main.c)
target_link_libraries(compiler compilerlib)

add_executable(lexer lexer_main.c)
target_link_libraries(lexer compilerlib)

add_executable(parser parser_main.c)
target_link_libraries(parser compilerlib)

add_executable(eval eval_main.c)
target_link_libraries(eval compilerlib)

add_executable(sema sema_main.c)
target_link_libraries(sema compilerlib)

#add_subdirectory(python)
