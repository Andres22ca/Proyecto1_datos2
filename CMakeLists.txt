cmake_minimum_required(VERSION 3.16)
project(libreria_instrumetalizacion)

set(CMAKE_CXX_STANDARD 17)

add_executable(libreria_instrumetalizacion
        Main.cpp
        Memory_Tracker.cpp
        SocketServer.cpp
        library.cpp
)
