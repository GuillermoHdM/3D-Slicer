#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include "Support.h"

float SupportSpacing = 0.5f;
float SupportRadius = 0.02f; //Support size (Option)
float BaseSize = 0.5f;
float BaseHeight = 0.05f;

void GenerateSupports(const std::vector<Triangle>& model, glm::mat4 TRS, std::vector<glm::vec3>& outSupports, std::vector<Triangle>& outSupportTriangles)
{
    auto world = ToWorldSpace(model, TRS);//pass to world space

    float maxAngle = glm::radians(45.0f);//what we consider an overhang
    std::unordered_set<GridKey, GridKeyHash> occupied;//to avoid multiple supports in the same "tile"
    std::unordered_map<GridKey, std::vector<glm::vec3>, GridKeyHash> baseClusters;//for support base aggregation
    for (const Triangle& tri : world)//for each triangle
    {
        float angle = acos(glm::dot(glm::normalize(tri.n), glm::vec3(0, 1, 0)));
        if (angle <= maxAngle)
            continue;

        glm::vec3 top = (tri.A + tri.B + tri.C) / 3.0f;//Center of the triangle

        GridKey key
        {
            int(floor(top.x / SupportSpacing)),
            int(floor(top.z / SupportSpacing))
        };
        //if its contained, ignore it
        if (!IsPointExposed(top, world))
        {
            continue;
        }

        if (occupied.contains(key))
            continue;

        glm::vec3 bottom;
        ProjectSinglePoint(top, world, bottom);

        occupied.insert(key);
        baseClusters[key].push_back(bottom);
        CreateSupportPillar(top, bottom, outSupports);

    }
    for (auto& [key, bots] : baseClusters)
    {
        glm::vec3 center(0.0f);
        for (auto& b : bots)
            center += b;

        center /= float(bots.size());

        CreateSupportBase(center, outSupports);
    }


    for (size_t i = 0; i < outSupports.size(); i += 3)
    {
        if (i + 2 < outSupports.size())
        { // Asegúrate de que hay suficientes vértices
            Triangle supportTri;
            supportTri.A = outSupports[i];
            supportTri.B = outSupports[i + 2];//avoid backfaces
            supportTri.C = outSupports[i + 1];
            supportTri.n = glm::normalize(glm::cross(supportTri.B - supportTri.A, supportTri.C - supportTri.A)); // Calcula la normal
            //supportTri.id = /* asigna un ID único */;

            outSupportTriangles.push_back(supportTri);
        }
    }

    glm::mat4 invTRS = glm::inverse(TRS);

    for (auto& tri : outSupportTriangles)
    {
        tri.A = glm::vec3(invTRS * glm::vec4(tri.A, 1.0f));
        tri.B = glm::vec3(invTRS * glm::vec4(tri.B, 1.0f));
        tri.C = glm::vec3(invTRS * glm::vec4(tri.C, 1.0f));

        tri.n = glm::normalize(glm::cross(tri.B - tri.A, tri.C - tri.A)); // recalcula normal
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
                    std::cout << "NaN on hit!!!\n";
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
            std::cout << "NaN on bestHit!!!\n";
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

    right *= SupportRadius;
    forward *= SupportRadius;

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
    addTri(t0, b0, b1);
    addTri(t0, b1, t1);
    addTri(t1, b1, b2);
    addTri(t1, b2, t2);
    addTri(t2, b2, b3);
    addTri(t2, b3, t3);
    addTri(t3, b3, b0);
    addTri(t3, b0, t0);
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



bool ProjectSinglePoint(const glm::vec3& top, const std::vector<Triangle>& world, glm::vec3& outBottom)
{
    glm::vec3 rayDir(0, -1, 0);
    float bestY = -FLT_MAX;
    bool hit = false;

    for (const Triangle& t : world)
    {
        float dist;
        glm::vec3 hitPoint;

        if (RayIntersectTriangle(top, rayDir, t, dist, hitPoint))
        {
            if (hitPoint.y < top.y && hitPoint.y > bestY)
            {
                bestY = hitPoint.y;
                outBottom = hitPoint;
                hit = true;
            }
        }
    }

    if (!hit)
        outBottom = glm::vec3(top.x, 0.0f, top.z); // bed

    return true;
}


void CreateSupportBase(const glm::vec3& bot, std::vector<glm::vec3>& outSupports)
{
    float h = BaseHeight;
    float s = BaseSize * 0.5f;

    glm::vec3 b0(bot.x - s, 0.0f, bot.z - s);
    glm::vec3 b1(bot.x + s, 0.0f, bot.z - s);
    glm::vec3 b2(bot.x + s, 0.0f, bot.z + s);
    glm::vec3 b3(bot.x - s, 0.0f, bot.z + s);

    glm::vec3 t0 = b0 + glm::vec3(0, h, 0);
    glm::vec3 t1 = b1 + glm::vec3(0, h, 0);
    glm::vec3 t2 = b2 + glm::vec3(0, h, 0);
    glm::vec3 t3 = b3 + glm::vec3(0, h, 0);

    auto addTri = [&](glm::vec3 A, glm::vec3 B, glm::vec3 C)
    {
        outSupports.push_back(A);
        outSupports.push_back(B);
        outSupports.push_back(C);
    };
    addTri(t0, t1, t2);
    addTri(t0, t2, t3);
    addTri(b0, b2, b1);
    addTri(b0, b3, b2);

    addTri(b0, b1, t1);
    addTri(b0, t1, t0);
    addTri(b1, b2, t2);
    addTri(b1, t2, t1);
    addTri(b2, b3, t3);
    addTri(b2, t3, t2);
    addTri(b3, b0, t0);
    addTri(b3, t0, t3);
}

bool IsPointExposed(const glm::vec3& p, const std::vector<Triangle>& world)
{
    glm::vec3 rayOrigin = p + glm::vec3(0, 0.001f, 0);
    glm::vec3 rayDir = glm::vec3(0, 1, 0);

    for (const Triangle& tri : world)
    {
        //quick discard
        if (tri.A.y < p.y && tri.B.y < p.y && tri.C.y < p.y)
            continue;
        //quick discard
        if (glm::dot(tri.n, glm::vec3(0, -1, 0)) <= 0.0f)
            continue;
        //with these small optimizations the suzanne binary test for the first triangle went from 0.0338ms to 0.0135ms 
        //not great not terrible
        float t;
        glm::vec3 hit;
        if (RayIntersectTriangle(rayOrigin, rayDir, tri, t, hit))
        {
            return true;
        }
    }
    return false;

}