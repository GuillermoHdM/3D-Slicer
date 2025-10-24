#include "Object.h"

Object::Object(std::vector<Triangle>& triangles) : m_Model(triangles)
{
    m_Transform.position = glm::vec3(0.0f);
    m_Transform.rotation = glm::vec3(0.0f);
    m_Transform.scale = glm::vec3(1.0f);

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
