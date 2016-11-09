cmake_minimum_required(VERSION 2.8)

set(RUN_TEST_PY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_test.py)

add_subdirectory(subexpressions)
add_subdirectory(eval)
add_subdirectory(sema)
add_subdirectory(intermediate)
add_subdirectory(unit_tests)
