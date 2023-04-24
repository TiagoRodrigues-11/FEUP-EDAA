cmake_minimum_required(VERSION 3.10)
project(KdTree)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -pthread")

add_executable(KdTree main.cpp Song.cpp Song.h)

add_subdirectory(libpqxx-7.7.5 build-pqxx)
target_link_libraries(KdTree PRIVATE pqxx)