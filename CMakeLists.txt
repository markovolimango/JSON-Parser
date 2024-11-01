cmake_minimum_required(VERSION 3.29)
project(JSON-Parser)

set(CMAKE_CXX_STANDARD 20)

add_executable(JSON-Parser main.cpp
        json.h
        json.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "json_eval")

add_executable(tests tests.cpp
        json.h
        json.cpp)
