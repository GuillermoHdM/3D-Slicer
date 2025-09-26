cmake_minimum_required(VERSION 3.11)
project(cs350-demo)

# Demo
add_executable(${PROJECT_NAME} main.cpp "Editor.cpp" "Editor.h")
target_include_directories(${PROJECT_NAME} PUBLIC .)

# Libraries
target_link_libraries(${PROJECT_NAME} PUBLIC cs350-lib)
