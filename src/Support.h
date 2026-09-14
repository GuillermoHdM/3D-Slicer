#pragma once
#include <vector>
#include "OpenGl.hpp"

struct SupportColumn
{
    std::vector<glm::vec3> Bot;
    std::vector<glm::vec3> Top;
};


void GenerateSupports(const std::vector<Triangle>& model, glm::mat4 TRS, std::vector<glm::vec3>& outSupports, std::vector<Triangle>& outSupportTriangles);
SupportColumn ProjectTriangle(const Triangle& tri, const std::vector<Triangle>& worldModel);
bool RayIntersectTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const Triangle& tri, float& out_t, glm::vec3& out_hit);
void CreateSupportPillar(const glm::vec3& contactTop, const glm::vec3& bodyTop, const glm::vec3& bot, std::vector<glm::vec3>& outSupports);
std::vector<Triangle> ToWorldSpace(const std::vector<Triangle>& model, const glm::mat4& TRS);
bool ProjectSinglePoint(const glm::vec3& top, const glm::vec3& normal, int CurrId, const std::vector<Triangle>& world, float supportRadius, glm::vec3& outBottom, glm::vec3& outAdjustedTop);
void CreateSupportBase(const glm::vec3& bot, std::vector<glm::vec3>& outSupports);
bool IsPointExposed(const glm::vec3& p, const std::vector<Triangle>& world);
bool IsPathClearDown(const glm::vec3& top, int CurrId,const std::vector<Triangle>& world);

//to know which supports are not needed (spacing)
struct GridKey 
{
    int x;
    int z;
    bool operator==(const GridKey& other) const {
        return x == other.x && z == other.z;
    }
};
//actually to know where in the "grid are they"
struct GridKeyHash 
{
    size_t operator()(const GridKey& k) const 
    {
        size_t h1 = std::hash<int>()(k.x);
        size_t h2 = std::hash<int>()(k.z);
        //Algorithm with negative values
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};