#pragma once
#include <vector>
#include "OpenGl.hpp"

struct SupportColumn
{
    std::vector<glm::vec3> Bot;
    std::vector<glm::vec3> Top;
};
void GenerateSupports(const std::vector<Triangle>& model, glm::mat4 TRS);
SupportColumn ProjectTriangle(const Triangle& tri, const std::vector<Triangle>& worldModel);
bool RayIntersectTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const Triangle& tri, float& out_t, glm::vec3& out_point);