#include "Editor.h"
#include "Shaders.h"
#include "OpenGl.hpp"
#include <random>
#include <iostream>
Editor::Editor() : W(ivec2(1920 * 0.75f, 1080 * 0.75f), true)
{
	Window::initialize_system();
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //tmp
    glm::mat4 view = glm::lookAt(
        glm::vec3(5.0f, 5.0f, 5.0f),  // camera position
        glm::vec3(0.0f, 0.0f, 0.0f),  // looking at origin
        glm::vec3(0.0f, 1.0f, 0.0f)   // up direction
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),  // FOV
        16.0f / 9.0f,         // aspect ratio (adjust if your window isn’t 16:9)
        0.1f,                 // near plane
        1000.0f               // far plane
    );
    //***
    m_Grid.Draw(view, projection);
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
