cmake_minimum_required(VERSION 3.19)
project(BuddySystem)

set(CMAKE_CXX_STANDARD 14)

add_executable(BuddySystem main.cpp)
add_executable(buddy buddySystem/buddy.c)
add_executable(buddyTest buddySystem/buddyTest.c)
INCLUDE_DIRECTORIES(buddySystem/buddy.h)