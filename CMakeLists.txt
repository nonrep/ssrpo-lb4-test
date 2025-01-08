cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(lab3)

SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

SET(CMAKE_CXX_FLAGS_RELEASE        "-O3")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 -g --coverage")
SET(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")

add_subdirectory(src/stressgen) 
add_subdirectory(src/lab) 
