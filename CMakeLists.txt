cmake_minimum_required(VERSION 3.16.3)

project(Prod_Cons_Problem)

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=gnu++14 -pthread")
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

include_directories(src)

file(GLOB SOURCES "src/*.cpp")

add_executable(prodcons ${SOURCES})