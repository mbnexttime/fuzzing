cmake_minimum_required(VERSION 3.24)
project(example)
set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_executable(runner runner.cpp)
target_link_libraries(runner LLVM)






