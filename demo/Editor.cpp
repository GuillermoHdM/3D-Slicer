#include "Editor.h"
#include "Shaders.h"
#include "OpenGl.hpp"
#include "FileLoader.h"
#include <random>
#include <iostream>
Editor::Editor() : W(ivec2(1920 * 0.75f, 1080 * 0.75f), true)
{
	Window::initialize_system();
    glfwSetWindowUserPointer(W.handle(), &m_Camera);
    glfwSetScrollCallback(W.handle(), ScrollCallback);
    glfwSetWindowUserPointer(W.handle(), this);
    glfwSetDropCallback(W.handle(), DropCallback);
	MyImgui.Init(W.handle());
    MyRenderer.Init();
    m_Grid.Init();
}
Editor::~Editor()
{
	MyImgui.Cleanup();
}
void Editor::Renderer::Init()
{
    VtxShader = CreateProgram(GL_VERTEX_SHADER, MyVertShader);
    FragShader = CreateProgram(GL_FRAGMENT_SHADER, MyFragShader);
    MyShader = glCreateProgram();
    glAttachShader(MyShader, VtxShader);
    glAttachShader(MyShader, FragShader);
    glLinkProgram(MyShader);

    glClearColor(0.00f, 0.0f, 0.0f, 0.0f);
}
void Editor::Renderer::Update()
{
}


bool Editor::Update()
{
	W.update();
	Dt = MyImgui.Update();
    m_Camera.Update(W.handle());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Grid.Draw(m_Camera.m_View, m_Camera.m_Projection);
    MyRenderer.Update();
	UpdateImGui();//get the display of imgui updated

    ivec2 windowSize = W.size();

	MyImgui.Draw();
	return !W.should_exit();
}


void Editor::UpdateImGui()
{
    if (ImGui::Begin("Editor"))
    {
        ImGui::End();
    }
}

void Editor::AddNewObject(std::vector<Triangle>& in_triangles)
{
    m_Objects.emplace_back(in_triangles);
}