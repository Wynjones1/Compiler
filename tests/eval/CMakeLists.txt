cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.x)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

foreach(TEST ${TESTS})
    set(TARGET_NAME test_eval_${TEST})
    add_executable(${TARGET_NAME} main.c)
    set(INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${TEST})
    target_compile_definitions(${TARGET_NAME} PUBLIC -DINPUT_FILE="${INPUT_FILE}")
    target_link_libraries(${TARGET_NAME} compilerlib)
    set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "Test/Eval")

    add_test(
        NAME "eval_${TEST}"
        COMMAND $<TARGET_FILE:${TARGET_NAME}>
    )
endforeach()
