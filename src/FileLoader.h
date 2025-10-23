#pragma once
#include <fstream>
#include <string>
#include "Math.hpp"
#include "Window.hpp"
//STL files have two possible types, STL Ascii and STL Binary
//https://www.fabbers.com/tech/STL_Format
bool IsBinary(const std::string& path);

void DropCallback(GLFWwindow* window, int count, const char** paths);

void LoadBinarySTL(const std::string& path, std::vector<Triangle>& out_triangles);
void LoadAsciiSTL(const std::string& path, std::vector<Triangle>& out_triangles);