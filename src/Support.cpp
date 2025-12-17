#include <iostream>
#include "Support.h"

void GenerateSupports(const std::vector<Triangle>& model, glm::mat4 TRS, std::vector<glm::vec3>& outSupports)
{
    //***get the model into world space***
    std::vector<Triangle> worldModel = ToWorldSpace(model, TRS);
    //************************************
    //***get triangles that need support by angle***
    float maxAngle = glm::radians(45.0f); // configurable (OPTION)

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
        for (int i = 0; i < 3; ++i)
        {
            glm::vec3 top = col.Top[i];
            glm::vec3 bot = col.Bot[i];

            CreateSupportPillar(top, bot, outSupports);
        }

    }
}


SupportColumn ProjectTriangle(const Triangle& tri, const std::vector<Triangle>& world)
{
    SupportColumn col;

    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 v = tri.A;
        if (i == 1) v = tri.B;
        if (i == 2) v = tri.C;

        float highestY = -FLT_MAX;
        glm::vec3 bestHit(v.x, 0.0f, v.z); // default floor hit

        glm::vec3 rayOrigin = v;
        glm::vec3 rayDir = glm::vec3(0, -1, 0);

        for (const Triangle& other : world)
        {
            if (other.id == tri.id)
                continue; // avoid self-collision

            float t;
            glm::vec3 hit;

            if (RayIntersectTriangle(rayOrigin, rayDir, other, t, hit))
            {
                if (glm::any(glm::isnan(hit))) {
                    std::cout << "NaN en hit!!!\n";
                }
                if (hit.y < v.y && hit.y > highestY)
                {
                    highestY = hit.y;
                    bestHit = hit;
                }
            }
        }

        col.Top.push_back(v);
        col.Bot.push_back(bestHit);
        if (glm::any(glm::isnan(bestHit))) {
            std::cout << "NaN en bestHit!!!\n";
        }
    }

    return col;
}
bool RayIntersectTriangle(const glm::vec3& rayOrigin,const glm::vec3& rayDir,const Triangle& tri,float& out_t,glm::vec3& out_hit)
{
    const float EPS = 1e-6f;

    glm::vec3 e1 = tri.B - tri.A;
    glm::vec3 e2 = tri.C - tri.A;
    glm::vec3 p = glm::cross(rayDir, e2);

    float det = glm::dot(e1, p);
    if (fabs(det) < EPS)
        return false; // parallel or no hit

    float invDet = 1.0f / det;
    glm::vec3 tvec = rayOrigin - tri.A;

    float u = glm::dot(tvec, p) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 q = glm::cross(tvec, e1);
    float v = glm::dot(rayDir, q) * invDet;
    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = glm::dot(e2, q) * invDet;
    if (t < 0.0f)
        return false; // hit is behind ray origin

    out_t = t;
    out_hit = rayOrigin + rayDir * t;
    return true;
}

void CreateSupportPillar(const glm::vec3& top, const glm::vec3& bot, std::vector<glm::vec3>& outSupports)
{
    float R = 0.005f; //Support size (Option)

    glm::vec3 axis = bot - top;
    if (glm::length(axis) < 1e-6f) {
        // columna nula, ignorarla
        return;
    }

    glm::vec3 dir = glm::normalize(axis);

    // si dir es paralelo a Y, usa otro up artificial
    glm::vec3 up = fabs(dir.y) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);

    glm::vec3 right = glm::normalize(glm::cross(dir, up));
    glm::vec3 forward = glm::normalize(glm::cross(dir, right));

    right *= R;
    forward *= R;

    //top 4 vert
    glm::vec3 t0 = top + right + forward;
    glm::vec3 t1 = top - right + forward;
    glm::vec3 t2 = top - right - forward;
    glm::vec3 t3 = top + right - forward;

    //bot 4 vert
    glm::vec3 b0 = bot + right + forward;
    glm::vec3 b1 = bot - right + forward;
    glm::vec3 b2 = bot - right - forward;
    glm::vec3 b3 = bot + right - forward;

    
    //Generate 12 triangles
    auto addTri = [&](glm::vec3 A, glm::vec3 B, glm::vec3 C)
    {
        outSupports.push_back(A);
        outSupports.push_back(B);
        outSupports.push_back(C);
    };

    //Top Face
    addTri(t0, t1, t2);
    addTri(t0, t2, t3);

    //Bot face
    addTri(b0, b2, b1);
    addTri(b0, b3, b2);
    //Sides
    addTri(t0, b0, b1);  addTri(t0, b1, t1);
    addTri(t1, b1, b2);  addTri(t1, b2, t2);
    addTri(t2, b2, b3);  addTri(t2, b3, t3);
    addTri(t3, b3, b0);  addTri(t3, b0, t0);
}

std::vector<Triangle> ToWorldSpace(const std::vector<Triangle>& model, const glm::mat4& TRS)
{
    std::vector<Triangle> world;
    world.reserve(model.size());

    for (const Triangle& tri : model)
    {
        Triangle t;
        t.A = glm::vec3(TRS * glm::vec4(tri.A, 1.0f));
        t.B = glm::vec3(TRS * glm::vec4(tri.B, 1.0f));
        t.C = glm::vec3(TRS * glm::vec4(tri.C, 1.0f));
        t.n = glm::normalize(glm::mat3(TRS) * tri.n);
        t.id = tri.id;   // preserve the ID

        world.push_back(t);
    }

    return world;
}