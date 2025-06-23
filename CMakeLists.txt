cmake_minimum_required(VERSION 3.25)
project(Concurrent_Programming)

set(CMAKE_CXX_STANDARD 17)

include_directories("Dining Philosophers Problem")

add_executable(Concurrent_Programming
        "Dining Philosophers Problem/dining_philosophers.cpp"
        "Dining Philosophers Problem/dining_philosophers.h"
        "main.cpp"
        "Readers-Writers Problem/readers_writers.h"
        "Readers-Writers Problem/readers_writers.cpp"
        )
