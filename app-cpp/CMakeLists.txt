cmake_minimum_required(VERSION 3.22)

project(sockets
        VERSION 1.0
        DESCRIPTION "C++ Client-Server Sockets Example"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(server src/Server.cpp src/Worker.cpp)

target_include_directories(server PUBLIC include)

add_executable(client src/Client.cpp)

target_include_directories(client PUBLIC include)