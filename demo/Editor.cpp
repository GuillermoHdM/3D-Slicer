#include "Editor.h"
#include "Shaders.h"
#include "OpenGl.hpp"
#include "FileLoader.h"
#include <random>
#include <iostream>
Editor::Editor() : W(ivec2(1920 * 0.75f, 1080 * 0.75f), true)
{
	Window::initialize_system();
    glfwSetWindowUserPointer(W.handle(), this);
    glfwSetScrollCallback(W.handle(), ScrollCallback);
    glfwSetDropCallback(W.handle(), DropCallback);
	MyImgui.Init(W.handle());
    R_Init();
    m_Grid.Init();
}
Editor::~Editor()
{
	MyImgui.Cleanup();
}
void Editor::R_Init()
{
    VtxShader = CreateProgram(GL_VERTEX_SHADER, MyVertShader);
    FragShader = CreateProgram(GL_FRAGMENT_SHADER, MyFragShader);
    MyShader = glCreateProgram();
    glAttachShader(MyShader, VtxShader);
    glAttachShader(MyShader, FragShader);
    glLinkProgram(MyShader);
    
    glClearColor(0.00f, 0.0f, 0.0f, 0.0f);
}
void Editor::R_Update()
{
    glUseProgram(MyShader);
    // Buscar ubicaciones de uniforms
    GLint locModel = glGetUniformLocation(MyShader, "model");
    GLint locView = glGetUniformLocation(MyShader, "view");
    GLint locProj = glGetUniformLocation(MyShader, "projection");
    GLint locColor = glGetUniformLocation(MyShader, "color");
    
    glUniformMatrix4fv(locView, 1, GL_FALSE, &m_Camera.m_View[0][0]);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &m_Camera.m_Projection[0][0]);
    glUniform4f(locColor, 0.9f, 0.9f, 0.9f, 1.0f);
    for (auto& obj : m_Objects)
    {
        glm::mat4 model = obj.m_Transform.modelMatrix;
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        obj.Draw(m_Config.m_Wireframe);
    }
    
    glUseProgram(0);
}


bool Editor::Update()
{
	W.update();
	Dt = MyImgui.Update();
    m_Camera.Update(W.handle());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Grid.Draw(m_Camera.m_View, m_Camera.m_Projection);
    R_Update();
	UpdateImGui();//get the display of imgui updated

    ivec2 windowSize = W.size();

	MyImgui.Draw();
	return !W.should_exit();
}


void Editor::UpdateImGui()
{
    if (ImGui::Begin("Editor"))
    {
        ImGui::Checkbox("Wireframe", &m_Config.m_Wireframe);
        ImGui::End();
    }
}

void Editor::AddNewObject(std::vector<Triangle>& in_triangles)
{
    m_Objects.push_back(Object(in_triangles));
    float spacing = 2.0f; // distancia entre objetos
    glm::vec3 newPosition = glm::vec3(m_Objects.size() * spacing, 0.0f, 0.0f);
    m_Objects.back().SetPosition(newPosition);
}