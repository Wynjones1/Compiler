cmake_minimum_required(VERSION 2.8)
project(compiler)

file(GLOB SRC "./src/*.c" "./src/*.h")
include_directories("./src/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/")

add_executable(compiler ${SRC})

if(MSVC)
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -std=c99 -O0 -Werror")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")

	set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)
	if(CMAKE_COMPILER_IS_GNUCXX)
		include(CodeCoverage)
		setup_target_for_coverage(coverage ${CMAKE_CURRENT_SOURCE_DIR}/bin/compiler output)
	endif()
endif()
