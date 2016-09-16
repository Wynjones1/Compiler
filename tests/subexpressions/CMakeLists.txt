cmake_minimum_required(VERSION 2.8)

# This should be in a find_package at some point. For now though this is fine...
include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

file( GLOB_RECURSE SUBEXPRESSION_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.x)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

foreach(FILE ${SUBEXPRESSION_TESTS})
    string(REGEX MATCH "(.*)/(.*)" OUT ${FILE})

    set(TARGET_NAME test_${CMAKE_MATCH_1}_${CMAKE_MATCH_2})
    add_executable(${TARGET_NAME} main.c)
    target_compile_definitions(${TARGET_NAME} PUBLIC -DPARSE_FUNC=${CMAKE_MATCH_1} PUBLIC -DPARSE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/${FILE}")
    target_link_libraries(${TARGET_NAME} compilerlib)
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER "Test/Subexpression")

    add_test(
        NAME    "${FILE}"
        COMMAND $<TARGET_FILE:${TARGET_NAME}>
    )
endforeach()
