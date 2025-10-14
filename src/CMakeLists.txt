cmake_minimum_required(VERSION 3.11)
project(cs350-lib)

# Engine library
add_library(
    ${PROJECT_NAME}
    Window.hpp
    Window.cpp
    OpenGl.hpp
    OpenGl.cpp
    
    Math.hpp            "MyImgui.cpp" "MyImgui.h"     "Shaders.h" "Grid.h" "Grid.cpp" "Shaders.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC .)

## ImGui
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)
# GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# GLAD
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glad::glad)

# GLFW3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
