/*!
******************************************************************************
\file    MyImgui.h
\author  Guillermo Hurtado de Mendoza
\par     DP email: guillermo.hurtado/@digipen.edu
\par     Course: CS350
\par     Programming Assignment Assignment 1
\date    11-02-2025

\brief
  Just my handler of imgui
*******************************************************************************/
#include <stdexcept>
#include <imgui.h>              // Imgui
#include <imgui_impl_glfw.h>    // Imgui
#include <imgui_impl_opengl3.h> // Imgui
#include "Window.hpp"

struct ImguiNode
{
	void Init(GLFWwindow* w);
	float Update();
	void Draw();
	void Cleanup();
};