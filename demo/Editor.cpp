#include "Editor.h"
#include <random>
#include <iostream>
Editor::Editor() : W(ivec2(1920 * 0.75f, 1080 * 0.75f), true)
{
	Window::initialize_system();
	MyImgui.Init(W.handle());
    MyRenderer.Init();

}
Editor::~Editor()
{
	MyImgui.Cleanup();
}
void Editor::Renderer::Init()
{
    VtxShader = CreateProgram(GL_VERTEX_SHADER);
    FragShader = CreateProgram(GL_FRAGMENT_SHADER);
    MyShader = glCreateProgram();
    glAttachShader(MyShader, VtxShader);
    glAttachShader(MyShader, FragShader);
    glLinkProgram(MyShader);
    glClearColor(0.00f, 0.0f, 0.0f, 0.0f);
}
GLuint Editor::Renderer::CreateProgram(GLenum stage)const
{
    GLuint Shader = glCreateShader(stage);
    if (stage == GL_VERTEX_SHADER)
    {
        glShaderSource(Shader, 1, &MyVertShader, nullptr);
    }
    else if (stage == GL_FRAGMENT_SHADER)
    {
        glShaderSource(Shader, 1, &MyFragShader, nullptr);
    }
    else
    {
        throw std::runtime_error("ERROR: Trying to create a shader I dont support");
    }
    glCompileShader(Shader);

    // Check for errors
    int IsOk = 0;
    char infoLog[512];
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsOk);
    if (!IsOk) {
        glGetShaderInfoLog(Shader, 512, nullptr, infoLog);
        throw std::runtime_error("ERROR: there was an error in the shader");
    }

    return Shader;
}


bool Editor::Update()
{
	W.update();
	Dt = MyImgui.Update();
	UpdateImGui();//gget the display of imgui updated

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
