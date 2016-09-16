cmake_minimum_required(VERSION 2.8)

find_package(PythonLibs REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})

add_executable(python_integration main.c)
target_link_libraries(python_integration ${PYTHON_LIBRARIES})

message(STATUS ${PYTHON_LIBRARIES})