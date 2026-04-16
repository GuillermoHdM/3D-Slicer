cmake_minimum_required(VERSION 3.11)
project(Slicer-lib)

# Engine library
add_library(
    ${PROJECT_NAME}
    Window.hpp
    Window.cpp
    OpenGl.hpp
    OpenGl.cpp
    
    Math.hpp            
    "MyImgui.cpp" "MyImgui.h"     
    "Shaders.h" "Shaders.cpp"
    "Grid.cpp" "Grid.h" 
    "Camera.h" "Camera.cpp" 
    "FileLoader.cpp" "FileLoader.h" 
    "Object.h" "Object.cpp" 
    "Slicing.cpp" "Slicing.h" 
    "Support.cpp" "Support.h" )

add_executable(Slicer
    main.cpp
    Editor.cpp
    Editor.h
)
target_link_libraries(Slicer PUBLIC ${PROJECT_NAME})
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
