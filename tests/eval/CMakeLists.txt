cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)
list_directories(TESTS ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(eval_test main.c)
target_link_libraries(eval_test compilerlib)

foreach(TEST ${TESTS})
    set(TARGET_NAME test_eval_${TEST})

    set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${TEST})
	
    add_test(
        NAME "eval<${TEST}>"
        COMMAND ${RUN_TEST_PY} $<TARGET_FILE:eval_test> ${TEST_DIR}
    )
endforeach()
