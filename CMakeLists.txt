cmake_minimum_required(VERSION 3.25)
project(weiboz_project1 C)

set(CMAKE_C_STANDARD 99)

add_executable(weiboz_project1 main.c linkedList.c linkedList.h memory.c memory.h)
