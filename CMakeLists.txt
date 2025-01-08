cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(lab4)

SET(CMAKE_CXX_FLAGS "-pthread -fPIC -fno-rtti -Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wduplicated-branches -Wduplicated-cond -Wextra-semi -Wfloat-equal -Wlogical-op -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wsign-promo")

SET(CMAKE_CXX_FLAGS_RELEASE        "-O3")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O0 -g --coverage")
SET(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")

add_subdirectory(src/stressgen) 
add_subdirectory(src/tp) 
add_subdirectory(src/lab) 
