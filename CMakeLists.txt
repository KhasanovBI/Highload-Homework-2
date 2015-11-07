cmake_minimum_required(VERSION 3.3)
project(CacheMeasuring)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0")

set(SOURCE_FILES main.cpp)
add_executable(CacheMeasuring ${SOURCE_FILES})