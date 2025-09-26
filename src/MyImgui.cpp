/*!
******************************************************************************
\file    MyImgui.cpp
\author  Guillermo Hurtado de Mendoza
\par     DP email: guillermo.hurtado/@digipen.edu
\par     Course: CS350
\par     Programming Assignment Assignment 1
\date    11-02-2025

\brief
  Just my handler of imgui
*******************************************************************************/
#include "MyImgui.h"

/******************************************************************************/
 /*!
   \brief
		initial setup of imgui
 */
 /******************************************************************************/
void ImguiNode::Init(GLFWwindow* w)
{
	IMGUI_CHECKVERSION();
	if (!ImGui::CreateContext())
		throw std::runtime_error("Could not initialize ImGui");

	if (!ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(w), true))
		throw std::runtime_error("Could not initialize ImGui::OpenGL");

	if (!ImGui_ImplOpenGL3_Init("#version 150"))
		throw std::runtime_error("Could not initialize ImGui::OpenGL (2)");
}
/******************************************************************************/
 /*!
   \brief
		updates the imgui
 */
 /******************************************************************************/
float ImguiNode::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	return ImGui::GetIO().DeltaTime;//return the dt to the scene

}
/******************************************************************************/
 /*!
   \brief
		renders what was setup
 */
 /******************************************************************************/
void ImguiNode::Draw()
{
	// UI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
/******************************************************************************/
 /*!
   \brief
		does the cleanup to avoid problems
 */
 /******************************************************************************/
void ImguiNode::Cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}