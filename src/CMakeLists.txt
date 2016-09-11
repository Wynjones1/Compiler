cmake_minimum_required(VERSION 2.8)

file(GLOB SRC *.c)
file(GLOB HDR *.h)

add_executable(compiler ${SRC} ${HDR})
