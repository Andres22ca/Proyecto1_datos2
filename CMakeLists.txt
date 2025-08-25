cmake_minimum_required(VERSION 4.0)
project(libreria_instrumetalizacion)

set(CMAKE_CXX_STANDARD 17)

add_library(libreria_instrumetalizacion SHARED library.cpp
        Memory_Tracker.cpp
        Memory_Tracker.h)
