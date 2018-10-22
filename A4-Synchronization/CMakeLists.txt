cmake_minimum_required(VERSION 3.12)
project(A4_Synchronization)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(A4_Synchronization
    BoundedBuffer.cpp
    BoundedBuffer.h
    client.cpp
    dataserver.cpp
    Histogram.cpp
    Histogram.h
    reqchannel.cpp
    reqchannel.h)
