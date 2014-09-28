cmake_minimum_required(VERSION 2.8)
project(compiler)

file(GLOB SRC "./src/*.c" "./src/*.h")
include_directories("./src/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../bin/")

add_executable(compiler ${SRC})

if(MSVC)
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -g -O0 -Werror")
endif()
