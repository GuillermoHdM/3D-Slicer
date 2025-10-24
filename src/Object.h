#pragma once
#include <vector>
#include "OpenGl.hpp"
class Object
{
public:
    Object(const std::vector<Triangle>& triangles);
    ~Object();
    struct Transform
    {
        glm::vec3 position;
        glm::vec3 rotation;//remember, radians are the true measure of angles in the universe
        glm::vec3 scale;
        glm::mat4 modelMatrix;
    } m_Transform;

    void SetPosition(const glm::vec3& pos);
    void SetRotation(const glm::vec3& rot);
    void SetScale(const glm::vec3& scale);
    void Draw(bool Wireframe);
    //I wish I didnt have to do this, but there was a problem of copying VAOs around when 
    //adding to the vector
    void SetOpenGlThings();
private:
	std::vector<Triangle> m_Model;
    void CalculateTransform();
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLsizei m_VertexCount = 0;
};