#include <iostream>
#include "Object.h"

Object::Object(const std::vector<Triangle>& triangles, std::string name) : m_Model(triangles), m_Name(name)
{
    m_Transform.position = glm::vec3(0.0f);
    m_Transform.rotation = glm::vec3(0.0f);
    m_Transform.scale = glm::vec3(1.0f);
    CalculateTransform();
    SetOpenGlThings();
}
Object::~Object()
{
    //ONLY destroy the VAO & VBO if they have not been copied
    if (m_VAO)
    {
        glDeleteVertexArrays(1, &m_VAO);
    }
    if (m_VBO)
    {
        glDeleteBuffers(1, &m_VBO);
    }
}

void Object::SetPosition(const glm::vec3& pos)
{ 
    m_Transform.position = pos; 
    CalculateTransform();
}
void Object::SetRotation(const glm::vec3& rot)
{ 
    m_Transform.rotation = rot; 
    CalculateTransform();
}
void Object::SetScale(const glm::vec3& scale)
{ 
    m_Transform.scale = scale; 
    CalculateTransform();
}
void Object::CalculateTransform()
{
    //TRS
    glm::mat4 T = glm::translate(glm::mat4(1.0f), m_Transform.position);
    glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), m_Transform.rotation.x, glm::vec3(1, 0, 0));
    glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), m_Transform.rotation.y, glm::vec3(0, 1, 0));
    glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), m_Transform.rotation.z, glm::vec3(0, 0, 1));
    glm::mat4 R = Rz * Ry * Rx; //matrixes have reversed order
    glm::mat4 S = glm::scale(glm::mat4(1.0f), m_Transform.scale);

    m_Transform.modelMatrix = T * R * S;
}
void Object::Draw(bool Wireframe, GLuint Shader)
{
    if (Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glBindVertexArray(m_VAO);

    if (m_VertexCountWithoutSupport == 0)
    {
        GLint locColor = glGetUniformLocation(Shader, "color");
        glUniform4f(locColor, 0.9f, 0.9f, 0.9f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    }
    else
    {
        GLint locColor = glGetUniformLocation(Shader, "color");
        glUniform4f(locColor, 0.9f, 0.9f, 0.9f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, m_VertexCountWithoutSupport);
        glUniform4f(locColor, 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, m_VertexCountWithoutSupport, m_VertexCount - m_VertexCountWithoutSupport);
    }
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//reset
}
void Object::DrawSupports(bool Wireframe)
{
    if (Wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (m_SupportVertices.empty())
        return;

    glBindVertexArray(m_SupportVAO);
    glDrawArrays(GL_TRIANGLES, 0, m_SupportVertices.size());
    glBindVertexArray(0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//reset

}
void Object::SetOpenGlThings()
{
    //OpenGl thingys
    std::vector<float> vertices;
    vertices.reserve(m_Model.size() * 9); // 3 vértices × 3 coords

    for (const auto& tri : m_Model)
    {
        vertices.push_back(tri.A.x); vertices.push_back(tri.A.y); vertices.push_back(tri.A.z);
        vertices.push_back(tri.n.x); vertices.push_back(tri.n.y); vertices.push_back(tri.n.z);
        vertices.push_back(tri.B.x); vertices.push_back(tri.B.y); vertices.push_back(tri.B.z);
        vertices.push_back(tri.n.x); vertices.push_back(tri.n.y); vertices.push_back(tri.n.z);
        vertices.push_back(tri.C.x); vertices.push_back(tri.C.y); vertices.push_back(tri.C.z);
        vertices.push_back(tri.n.x); vertices.push_back(tri.n.y); vertices.push_back(tri.n.z);

    }
    if(m_VertexCount == 0)
        m_VertexCount = static_cast<GLsizei>(vertices.size() / 6);
    else
    {
        m_VertexCountWithoutSupport = m_VertexCount;
        m_VertexCount = static_cast<GLsizei>(vertices.size() / 6);
    }
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    //normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

Object::Object(Object&& other) noexcept
{
    ActualMove(std::move(other));
}
void Object::ActualMove(Object&& other) noexcept
{
    m_Model = std::move(other.m_Model);
    m_VAO = other.m_VAO;
    m_VBO = other.m_VBO;
    m_SupportVAO = other.m_SupportVAO;
    m_SupportVBO = other.m_SupportVBO;

    m_VertexCount = other.m_VertexCount;
    m_Transform = other.m_Transform;
    m_Name = std::move(other.m_Name);
    // deactivate the other
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_SupportVAO = 0;
    other.m_SupportVBO = 0;
    other.m_VertexCount = 0;
}

void Object::SetSupportsGL()
{
    /*if (m_SupportVAO == 0)
        glGenVertexArrays(1, &m_SupportVAO);
    if (m_SupportVBO == 0)
        glGenBuffers(1, &m_SupportVBO);

    glBindVertexArray(m_SupportVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SupportVBO);

    glBufferData(GL_ARRAY_BUFFER,
        m_SupportVertices.size() * sizeof(glm::vec3),
        m_SupportVertices.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
        (void*)0);

    glBindVertexArray(0);*/
}
