cmake_minimum_required(VERSION 2.8)

project(eventloop CXX)
set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb -std=c++11")#就是这两行东东
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_COMPILER "g++")

include_directories(./net)
add_subdirectory(net)

set(SRC 
    test_eventloop.cpp
)

add_executable(eventloop ${SRC})
target_link_libraries(eventloop sapper pthread)

