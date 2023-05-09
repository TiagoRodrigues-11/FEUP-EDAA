cmake_minimum_required(VERSION 3.10)
project(KdTree)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -pthread -lpqxx -lpq")

add_executable(KdTree main.cpp Song.cpp Song.h utils.cpp utils.h)

find_package(libpqxx REQUIRED)
target_link_libraries(KdTree libpqxx::pqxx)