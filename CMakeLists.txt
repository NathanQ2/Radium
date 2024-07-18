cmake_minimum_required(VERSION 3.28)
project(Radium)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)

add_executable(Radium ${SOURCES})
