cmake_minimum_required(VERSION 3.7)
project(Pos_chat C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(server server.c)
add_executable(klient klient.c)

