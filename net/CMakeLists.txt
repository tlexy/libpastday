cmake_minimum_required(VERSION 2.8)

#project(eventloop CXX)
set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb -std=c++11")#就是这两行东东
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_COMPILER "g++")

set(SRC 
../base/common.hpp
../base/condition.hpp
../base/current_thread.h
../base/mutex.hpp
acceptor.cpp 
base_handle.cpp 
buffer_ptr.cpp
epoller.cpp 
event_loop.cpp 
ip_address.cpp 
queue_buffer.h 
sapper.cpp 
socket_server.cpp 
socket_util.cpp 
socket.cpp 
)

#add_executable(eventloop test_eventloop.cpp base/fair_memory_pool.cpp base/timer_mgr.cpp timer_event_loop.cpp sapper.cpp socket.cpp socket_util.cpp ip_address.cpp epoller.cpp base/thread.cpp base/date_time.cpp base/mutex.hpp poller.h)
#target_link_libraries(eventloop pthread)
add_library(sapper STATIC ${SRC})

