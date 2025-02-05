cmake_minimum_required(VERSION 3.29)
project(JSON-Parser)

set(CMAKE_CXX_STANDARD 20)

add_executable(json-eval src/main.cpp
        include/json.h
        src/json.cpp
)

add_executable(tests tests/tests.cpp
        include/json.h
        src/json.cpp)
