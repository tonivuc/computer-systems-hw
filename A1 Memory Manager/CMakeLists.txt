cmake_minimum_required(VERSION 3.7)
project(A1_Memory_Manager)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp LinkedList.h BuddyAllocator.cpp BuddyAllocator.h BlockHeader.h BlockHeader.cpp LinkedList.cpp test/Ackerman.h test/Ackerman.cpp)
add_executable(A1_Memory_Manager ${SOURCE_FILES})