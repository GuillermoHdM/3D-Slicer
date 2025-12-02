#include "Support.h"

void GenerateSupports(const std::vector<Triangle>& model, glm::mat4 TRS)
{
    //***get the model into world space***
    std::vector<Triangle> worldModel;
    worldModel.reserve(model.size());

    for (auto& tri : model)
    {
        Triangle t;
        t.A = glm::vec3(TRS * glm::vec4(tri.A, 1.0));
        t.B = glm::vec3(TRS * glm::vec4(tri.B, 1.0));
        t.C = glm::vec3(TRS * glm::vec4(tri.C, 1.0));
        t.n = glm::normalize(glm::mat3(TRS) * tri.n);

        worldModel.push_back(t);
    }
    //************************************
    //***get triangles that need support by angle***
    float maxAngle = glm::radians(45.0f); // configurable

    std::vector<const Triangle*> overhangTriangles;

    for (auto& tri : worldModel)
    {
        float dp = glm::dot(glm::normalize(tri.n), glm::vec3(0, 1, 0));
        float angle = acos(dp);

        if (angle > maxAngle)
            overhangTriangles.push_back(&tri);
    }
    //************************************
    //Proyect down the triangles to actually get the support volume
    std::vector<SupportColumn> supportColumns;

    for (auto* tri : overhangTriangles)
    {
        SupportColumn col = ProjectTriangle(*tri, worldModel);
        if (!col.Bot.empty())
            supportColumns.push_back(col);
    }
    //***************************************************************
}


SupportColumn ProjectTriangle(const Triangle& tri, const std::vector<Triangle>& worldModel)
{
    SupportColumn col;

    glm::vec3 verts[3] = { tri.A, tri.B, tri.C };
    glm::vec3 down = glm::vec3(0, -1, 0);

    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 origin = verts[i];
        glm::vec3 hitPoint;
        float bestT = FLT_MAX;
        bool hit = false;

        //searchh the closest triangle under it
        for (const Triangle& other : worldModel)
        {
            //avoid self collition
            if (&other == &tri)
                continue;

            float t;
            glm::vec3 pt;

            if (RayIntersectTriangle(origin, down, other, t, pt))
            {
                if (t < bestT)
                {
                    bestT = t;
                    hitPoint = pt;
                    hit = true;
                }
            }
        }

        if (hit)
        {
            col.Bot.push_back(hitPoint);
        }
        else
        {
            // No collition, reaches the ground
            glm::vec3 bed = origin;
            bed.y = 0.0f;
            col.Bot.push_back(bed);
        }

        col.Top.push_back(origin);
    }

    return col;
}
bool RayIntersectTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const Triangle& tri, float& out_t, glm::vec3& out_point)
{
    const float EPS = 1e-6f;

    glm::vec3 e1 = tri.B - tri.A;
    glm::vec3 e2 = tri.C - tri.A;
    glm::vec3 p = glm::cross(rayDir, e2);

    float det = glm::dot(e1, p);
    if (fabs(det) < EPS) return false; //no intersection

    float invDet = 1.0f / det;
    glm::vec3 tvec = rayOrigin - tri.A;

    float u = glm::dot(tvec, p) * invDet;
    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 q = glm::cross(tvec, e1);
    float v = glm::dot(rayDir, q) * invDet;
    if (v < 0.0f || u + v > 1.0f) return false;

    float t = glm::dot(e2, q) * invDet; // Ray dist

    if (t < 0) return false; //Only down

    out_t = t;
    out_point = rayOrigin + t * rayDir;
    return true;
}