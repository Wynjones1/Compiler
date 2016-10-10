cmake_minimum_required(VERSION 2.8)
project(compiler)

include(cmake/setup.cmake)
include(cmake/functions.cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(dependencies)
add_subdirectory(src)

if(NOT MSVC)
	add_custom_target(run
		COMMAND $<TARGET_FILE:eval> ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple.x
		DEPENDS compiler
	)

	add_custom_target(dbg
		COMMAND cgdb --args $<TARGET_FILE:compiler> ${CMAKE_CURRENT_SOURCE_DIR}/examples/simple.x
		DEPENDS compiler
	)
endif()

enable_testing()
file(GLOB TESTS examples/*.x)

foreach(FILE ${TESTS})
    add_test(NAME ${FILE}
        COMMAND $<TARGET_FILE:parser> ${FILE}
    )
endforeach()


add_subdirectory(tests)
