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
    
    //Debug Slice vvvv
    SlicesVtxShader = CreateProgram(GL_VERTEX_SHADER, SliceVtxShader);
    SlicesFragShader = CreateProgram(GL_FRAGMENT_SHADER, SliceFragShader);
    SlicesShader = glCreateProgram();
    glAttachShader(SlicesShader, SlicesVtxShader);
    glAttachShader(SlicesShader, SlicesFragShader);
    glLinkProgram(SlicesShader);

    glGenTextures(1, &SliceDebuTex);
    glBindTexture(GL_TEXTURE_2D, SliceDebuTex);
    //                                        w     h
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenFramebuffers(1, &SliceDebugFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, SliceDebugFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SliceDebuTex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //^^^^^^^^^^^^^^^^
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClearColor(0.00f, 0.0f, 0.0f, 0.0f);

}
void Editor::R_Update()
{
    if (m_Config.m_Wireframe)//to see the faces on the wireframe
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    }
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
    if (m_Config.m_SliceDebug)
    {                                                                          //  m_Config.m_CurrSlice      1.0f (layer heignt)
        DrawSliceDebug(m_Config.DebugSlices[m_Config.m_CurrSlice], SlicesShader, m_Config.m_CurrSlice * m_Config.layerHeight);
    }
    glUseProgram(0);
}


bool Editor::Update()
{
	W.update();
	Dt = MyImgui.Update();
    m_Camera.Update(W.handle());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    R_Update();
    m_Grid.Draw(m_Camera.m_View, m_Camera.m_Projection);
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
        ImGui::Checkbox("Show Slices", &m_Config.m_SliceDebug);
        if (m_Config.m_SliceDebug && m_Objects.size() != 0)
        {
            ImGui::Text("Select Object:");
            if (ImGui::Button("<")) 
            {
                if (m_Config.m_SelectedObject > 0)
                {
                    m_Config.m_SelectedObject--;
                    m_Config.DebugSlices.clear();
                }
            }
            ImGui::SameLine();
            ImGui::Text("Object %d / %d", m_Config.m_SelectedObject + 1, (int)m_Objects.size());
            ImGui::SameLine();
            if (ImGui::Button(">")) 
            {
                if (m_Config.m_SelectedObject < m_Objects.size() - 1)
                {
                    m_Config.m_SelectedObject++;
                    m_Config.DebugSlices.clear();
                }
            }
            if (m_Config.DebugSlices.empty())//not sliced yet
            {
                m_Config.DebugSlices = GenerateMeshSlices(m_Objects[m_Config.m_SelectedObject].m_Model, m_Config.layerHeight);
            }
            if (ImGui::Button("-"))
            {
                if (m_Config.m_CurrSlice > 0)
                    m_Config.m_CurrSlice--;
            }
            if (ImGui::Button("+")) 
            { 
                if (m_Config.m_CurrSlice < m_Config.DebugSlices.size() - 1)
                    m_Config.m_CurrSlice++;
            }
            ImGui::SameLine();
            ImGui::Text("Layer %d / %d", m_Config.m_CurrSlice + 1, (int)m_Config.DebugSlices.size());

            ImGui::Image((ImTextureID)(intptr_t)SliceDebuTex, ImVec2(512, 512));
        }

        for (size_t i = 0; i < m_Objects.size(); i++)
        {
            ImGui::PushID(static_cast<int>(i));
            ImGui::Text(m_Objects[i].m_Name.c_str());
            bool changed = false;
            changed |= ImGui::InputFloat3("Position", &m_Objects[i].m_Transform.position.x);
            changed |= ImGui::InputFloat3("Rotation", &m_Objects[i].m_Transform.rotation.x);
            changed |= ImGui::InputFloat3("Scale", &m_Objects[i].m_Transform.scale.x);
            ImGui::Separator();
            ImGui::PopID();
            if (changed)
                m_Objects[i].CalculateTransform();
        }
        ImGui::End();
    }
}

void Editor::AddNewObject(std::vector<Triangle>& in_triangles, std::string name)
{
    m_Objects.push_back(Object(in_triangles, name));
    float spacing = 2.0f; // distancia entre objetos
    glm::vec3 newPosition = glm::vec3(m_Objects.size() * spacing, 0.0f, 0.0f);
    m_Objects.back().SetPosition(newPosition);
}

void Editor::DrawSliceDebug(const MeshSlice& slice, GLuint shader, float zOffset)
{
    //save our main viewport
    GLint prevViewport[4];//remember opengl defines a viewport as x offset, y offset, w and h
    glGetIntegerv(GL_VIEWPORT, prevViewport);

    //bind our debug viewport
    glBindFramebuffer(GL_FRAMEBUFFER, SliceDebugFBO);
    //prepare to draw
    glViewport(0, 0, 512, 512);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //lets center it
    float minX = FLT_MAX, minY = FLT_MAX;
    float maxX = -FLT_MAX, maxY = -FLT_MAX;

    for (auto& contour : slice)
    {
        for (auto& p : contour)
        {
            minX = std::min(minX, p.x);
            minY = std::min(minY, p.y);
            maxX = std::max(maxX, p.x);
            maxY = std::max(maxY, p.y);
        }
    }

    float w = maxX - minX;
    float h = maxY - minY;
    float maxDim = std::max(w, h);

    //santiy check
    if (maxDim < 0.0001f)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
        return;
    }
    float scale = 1.8f / maxDim;
    glm::vec2 center(minX + w * 0.5f, minY + h * 0.5f);

    //build and center the vertices from our slices
    std::vector<glm::vec3> vertices;
    vertices.reserve(slice.size() * 32);
    for (const auto& contour : slice)
    {
        for (size_t i = 0; i < contour.size() - 1; ++i)
        {
            glm::vec2 a = (contour[i] - center) * scale;
            glm::vec2 b = (contour[i + 1] - center) * scale;

            vertices.push_back({ a.x, a.y, 0.0f });
            vertices.push_back({ b.x, b.y, 0.0f });
        }
    }
    //if nothing to draw early exit
    if (vertices.empty())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
        return;
    }

    //prepare to OpenGl draw
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glUseProgram(shader);

    //set uniforms
    glm::mat4 MVP = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    GLint locMVP = glGetUniformLocation(shader, "u_MVP");
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, &MVP[0][0]);
    GLint locColor = glGetUniformLocation(shader, "u_Color");
    glUniform4f(locColor, 0.1f, 0.6f, 1.0f, 0.9f);

    glLineWidth(1.0f);//careful, drivers dont like weird values here!
    glDrawArrays(GL_LINES, 0, (GLsizei)vertices.size());

    //recover previous viewport and draw
    glBindVertexArray(0);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);//recover previous viewport
}
