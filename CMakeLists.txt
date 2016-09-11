cmake_minimum_required(VERSION 2.8)

add_subdirectory(src)

add_custom_target(run
    COMMAND $<TARGET_FILE:compiler> ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple.x
    DEPENDS compiler
)

add_custom_target(dbg
    COMMAND cgdb --args $<TARGET_FILE:compiler> ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple.x
    DEPENDS compiler
)
