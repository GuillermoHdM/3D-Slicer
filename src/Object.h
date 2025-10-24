#pragma once
#include <vector>
#include "OpenGl.hpp"
class Object
{
public:
    Object(std::vector<Triangle>& triangles);
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
private:
	std::vector<Triangle> m_Model;
    void CalculateTransform();

};