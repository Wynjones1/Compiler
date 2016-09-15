cmake_minimum_required(VERSION 2.8)

add_subdirectory(src)

add_custom_target(run
    COMMAND $<TARGET_FILE:eval> ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple.x
    DEPENDS compiler
)

add_custom_target(dbg
    COMMAND cgdb --args $<TARGET_FILE:compiler> ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple.x
    DEPENDS compiler
)

enable_testing()
file(GLOB TESTS examples/*.x)

foreach(FILE ${TESTS})
    add_test(NAME ${FILE}
        COMMAND $<TARGET_FILE:parser> ${FILE}
    )
endforeach()

file(
    GLOB_RECURSE SUBEXPRESSION_TESTS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/tests/subexpressions
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/subexpressions/*.x
)

foreach(FILE ${SUBEXPRESSION_TESTS})
    string(REGEX MATCH "(.*)/(.*)" OUT ${FILE})

    add_test(NAME "${FILE}"
        COMMAND
            $<TARGET_FILE:parser_test>
                ${CMAKE_MATCH_1}
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/subexpressions/${FILE}
    )
endforeach()
