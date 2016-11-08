cmake_minimum_required(VERSION 2.8)

set(TEST_FILES
    lexer.cpp
    parser.cpp
    subexpressions.cpp
)

find_package(Compiler REQUIRED)
find_package(GTest    REQUIRED)

include_directories(
    ${GTEST_INCLUDE_DIRECTORIES}
    ${COMPILER_INCLUDE_DIRECTORIES}
)

message(STATUS ${COMPILER_INCLUDE_DIRECTORIES})

add_executable(unit_tests ${TEST_FILES})
target_link_libraries(unit_tests ${GTEST_LIBRARIES} ${COMPILER_LIBRARIES})

add_test(NAME UnitTests
    COMMAND $<TARGET_FILE:unit_tests>
)

add_custom_target(
    run_gtests
    COMMAND $<TARGET_FILE:unit_tests>
)
