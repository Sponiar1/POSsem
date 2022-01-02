cmake_minimum_required(VERSION 3.7.2)
project(semka C)

set(CMAKE_C_STANDARD 99)

add_executable(semka main.c)
add_executable(server server.c)
add_executable(client client.c)