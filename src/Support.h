#pragma once
#include <vector>
#include "OpenGl.hpp"

struct SupportColumn
{
    std::vector<glm::vec3> Bot;
    std::vector<glm::vec3> Top;
};
void GenerateSupports(const std::vector<Triangle>& model, glm::mat4 TRS, std::vector<Triangle>& outSupports);
SupportColumn ProjectTriangle(const Triangle& tri, const std::vector<Triangle>& worldModel);
bool RayIntersectTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const Triangle& tri, float& out_t, glm::vec3& out_hit);
void CreateSupportPillar(const glm::vec3& top, const glm::vec3& bot, std::vector<Triangle>& outSupports);
std::vector<Triangle> ToWorldSpace(const std::vector<Triangle>& model, const glm::mat4& TRS);