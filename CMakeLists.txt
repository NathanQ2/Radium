cmake_minimum_required(VERSION 3.28)
project(Radium)

set(CMAKE_CXX_STANDARD 23)

find_package(spdlog REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h)

add_executable(Radium ${SOURCES}
        src/Reader.h)

target_link_libraries(Radium PRIVATE spdlog::spdlog)
