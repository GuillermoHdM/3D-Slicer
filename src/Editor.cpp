#include "Editor.h"
#include "Shaders.h"
#include "OpenGl.hpp"
#include "FileLoader.h"
#include "Support.h"
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
    //Regular shaders---
    VtxShader = CreateProgram(GL_VERTEX_SHADER, MyVertShader);
    FragShader = CreateProgram(GL_FRAGMENT_SHADER, MyFragShader);
    MyShader = glCreateProgram();
    glAttachShader(MyShader, VtxShader);
    glAttachShader(MyShader, FragShader);
    glLinkProgram(MyShader);

    //Slice shaders---
    SlicesVtxShader = CreateProgram(GL_VERTEX_SHADER, SliceVtxShader);
    SlicesFragShader = CreateProgram(GL_FRAGMENT_SHADER, SliceFragShader);
    SlicesShader = glCreateProgram();
    glAttachShader(SlicesShader, SlicesVtxShader);
    glAttachShader(SlicesShader, SlicesFragShader);
    glLinkProgram(SlicesShader);

    //Frame buffer---
    glGenTextures(1, &SliceDebuTex);
    glBindTexture(GL_TEXTURE_2D, SliceDebuTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    //avoid interpolations here
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &SliceDebugFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, SliceDebugFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SliceDebuTex, 0);

    //stencil buffer to keep track of edes crossed
    glGenRenderbuffers(1, &SliceDebugRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, SliceDebugRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, SliceDebugRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Error: SliceDebugFBO no está completo!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Slices---
    //Contours data
    glGenVertexArrays(1, &SliceContourVAO);
    glGenBuffers(1, &SliceContourVBO);
    glBindVertexArray(SliceContourVAO);
    glBindBuffer(GL_ARRAY_BUFFER, SliceContourVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    //Slice canvas data
    glGenVertexArrays(1, &BedQuadVAO);
    glGenBuffers(1, &BedQuadVBO);
    glBindVertexArray(BedQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, BedQuadVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glBindVertexArray(0);

    //Return state---
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
    
    glUniformMatrix4fv(locView, 1, GL_FALSE, &m_Camera.m_View[0][0]);
    glUniformMatrix4fv(locProj, 1, GL_FALSE, &m_Camera.m_Projection[0][0]);
    for (auto& obj : m_Objects)
    {
        glm::mat4 model = obj.m_Transform.modelMatrix;
        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);
        obj.Draw(m_Config.m_Wireframe, MyShader);
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
                m_Config.DebugSlices = GenerateMeshSlices(m_Objects[m_Config.m_SelectedObject].m_Model, m_Config.layerHeight, m_Objects[m_Config.m_SelectedObject].m_Transform.modelMatrix);
            }
            else
            {
                int totalSlices = (int)m_Config.DebugSlices.size();
                int currentLayer1Based = m_Config.m_CurrSlice + 1;

                std::string formatStr = "Layer %d / " + std::to_string(totalSlices);

                if (ImGui::SliderInt("Slice", &currentLayer1Based, 1, totalSlices, formatStr.c_str()))
                {
                    m_Config.m_CurrSlice = currentLayer1Based - 1;
                }
            }

            ImGui::Image((void*)(intptr_t)SliceDebuTex, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
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

        if (ImGui::Button("Generate Supports"))
        {
            float BaseHeight = 0.6f; //this parameter exists on Support.cpp, I must unify it NOW!!!
            m_Objects[m_Config.m_SelectedObject].m_Transform.position.y += BaseHeight + m_Config.layerHeight * 2;
            m_Objects[m_Config.m_SelectedObject].CalculateTransform();
            GenerateSupports(m_Objects[m_Config.m_SelectedObject].m_Model, m_Objects[m_Config.m_SelectedObject].m_Transform.modelMatrix, m_Objects[m_Config.m_SelectedObject].m_SupportVertices, m_Objects[m_Config.m_SelectedObject].m_SupportTriangles);
            m_Objects[m_Config.m_SelectedObject].m_Model.insert(m_Objects[m_Config.m_SelectedObject].m_Model.end(), m_Objects[m_Config.m_SelectedObject].m_SupportTriangles.begin(), m_Objects[m_Config.m_SelectedObject].m_SupportTriangles.end());

            //m_Objects[m_Config.m_SelectedObject].SetSupportsGL();
            m_Objects[m_Config.m_SelectedObject].SetOpenGlThings();
        }
        if (ImGui::Button("Spawn Suzanne"))
        {
            std::string path = "../Suzanne.stl";
            std::vector<Triangle> triangles;
            LoadAsciiSTL(path,triangles);
            AddNewObject(triangles, "Suzanne");
            m_Objects.back().SetScale(glm::vec3(12.0, 12.0, 12.0));
        }
        if (ImGui::Button("Spawn Smooth Suzanne"))
        {
            std::string path = "../SuzanneBinary.stl";
            std::vector<Triangle> triangles;
            LoadBinarySTL(path, triangles);
            AddNewObject(triangles, "Smooth Suzanne");
            m_Objects.back().SetScale(glm::vec3(12.0, 12.0, 12.0));
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
    GLint prevViewport[4];
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    glBindFramebuffer(GL_FRAMEBUFFER, SliceDebugFBO);
    glViewport(0, 0, 512, 512);

    //Uncured Resin (Backgground)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //If no contours, nothing to do
    if (slice.empty())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
        return;
    }

    //Cured Resin
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);

    glUseProgram(shader);

    //Orthographic projection
    float bedWidth = (m_Config.m_bedWidth > 0.0f) ? m_Config.m_bedWidth : 220.0f;
    float bedDepth = (m_Config.m_bedDepth > 0.0f) ? m_Config.m_bedDepth : 220.0f;
    float halfW = bedWidth * 0.5f;
    float halfD = bedDepth * 0.5f;
    glm::mat4 projection = glm::ortho(-halfW, halfW, -halfD, halfD, -1.0f, 1.0f);

    GLint locProj = glGetUniformLocation(shader, "projection");
    if (locProj == -1)
        locProj = glGetUniformLocation(shader, "u_MVP");
    if (locProj != -1) 
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

    GLint locColor = glGetUniformLocation(shader, "uColor");
    if (locColor == -1) 
        locColor = glGetUniformLocation(shader, "u_Color");


    //First Pass: cummulation
    glEnable(GL_STENCIL_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_CULL_FACE);

    //Always on the stencil
    glStencilFunc(GL_ALWAYS, 0, 0xFF);

    //we increase the value
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    glBindVertexArray(SliceContourVAO);
    glBindBuffer(GL_ARRAY_BUFFER, SliceContourVBO);

    for (const auto& contour : slice)
    {
        if (contour.size() < 3) 
            continue;
        glBufferData(GL_ARRAY_BUFFER, contour.size() * sizeof(glm::vec2), contour.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(contour.size()));
    }

    //Fiill the quad
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    //Paint anything with stencil
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    if (locColor != -1)
    {
        glUniform4f(locColor, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    //Entire quad
    glm::vec2 bedQuadVertices[6] = {
        { -halfW, -halfD },
        {  halfW, -halfD },
        {  halfW,  halfD },

        { -halfW, -halfD },
        {  halfW,  halfD },
        { -halfW,  halfD }
    };

    glBindVertexArray(BedQuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, BedQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bedQuadVertices), bedQuadVertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //Recover OpenGl state after all this
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBindVertexArray(0);
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
}
