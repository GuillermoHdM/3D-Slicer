#include <iostream>
#include "Object.h"

Object::Object(const std::vector<Triangle>& triangles) : m_Model(triangles)
{
    m_Transform.position = glm::vec3(0.0f);
    m_Transform.rotation = glm::vec3(0.0f);
    m_Transform.scale = glm::vec3(1.0f);
    CalculateTransform();
}
Object::~Object()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
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
void Object::Draw(bool Wireframe)
{
    std::cout << "[Object::Draw] binding VAO=" << m_VAO << " VBO=" << m_VBO << "\n";
    glBindVertexArray(m_VAO);
    if (!Wireframe)
    {
        glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    }
    else
    {
        glDrawArrays(GL_LINES, 0, m_VertexCount);
    }
    glBindVertexArray(0);
}

void Object::SetOpenGlThings()
{

    //OpenGl thingys
    std::vector<float> vertices;
    vertices.reserve(m_Model.size() * 9); // 3 vértices × 3 coords

    for (const auto& tri : m_Model)
    {
        vertices.push_back(tri.A.x); vertices.push_back(tri.A.y); vertices.push_back(tri.A.z);
        vertices.push_back(tri.B.x); vertices.push_back(tri.B.y); vertices.push_back(tri.B.z);
        vertices.push_back(tri.C.x); vertices.push_back(tri.C.y); vertices.push_back(tri.C.z);
    }

    m_VertexCount = static_cast<GLsizei>(vertices.size() / 3);
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    std::cout << "[Object] Created VAO=" << m_VAO << " VBO=" << m_VBO << " vertexCount=" << m_VertexCount << "\n";
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

