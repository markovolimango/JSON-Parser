cmake_minimum_required(VERSION 3.29)
project(JSON-Parser)

set(CMAKE_CXX_STANDARD 20)

add_executable(JSON-Parser main.cpp
        json.h
        json.cpp
)
