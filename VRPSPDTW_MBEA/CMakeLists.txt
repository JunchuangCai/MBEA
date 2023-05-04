cmake_minimum_required(VERSION 3.20)
project(VRPHTO_clion)

set(CMAKE_CXX_STANDARD 14)

add_executable(MBEA arrayOperations.cpp functions.h initialize.cpp MFO.cpp mfoOperations.cpp searchOperators.cpp)
