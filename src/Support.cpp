#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include "Support.h"

float SupportSpacing = 3.0f;
float SupportRadius = 0.3f; //Support size (Option)
float BaseSize = 6.0f;
float BaseHeight = 0.6f;

void GenerateSupports(const std::vector<Triangle>& model, glm::mat4 TRS, std::vector<glm::vec3>& outSupports, std::vector<Triangle>& outSupportTriangles)
{
    auto world = ToWorldSpace(model, TRS);//pass to world space

    float maxAngle = glm::radians(45.0f);//what we consider an overhang
    std::unordered_set<GridKey, GridKeyHash> occupied;//to avoid multiple supports in the same "tile"
    std::unordered_map<GridKey, std::vector<glm::vec3>, GridKeyHash> baseClusters;//for support base aggregation
    for (const Triangle& tri : world)//for each triangle
    {
        float d = glm::dot(glm::normalize(tri.n), glm::vec3(0, 1, 0));
        bool isOverhang = d < -cos(maxAngle);
        if (!isOverhang)
            continue;
        float area = glm::length(glm::cross(tri.B - tri.A, tri.C - tri.A)) * 0.5f;
        int steps = std::max(1, int(std::ceil(std::sqrt(area) / SupportSpacing)));
        for (int i = 0; i <= steps; ++i)
        {
            for (int j = 0; i + j <= steps; ++j)
            {
                float u = (float)i / steps;
                float v = (float)j / steps;
                float w = 1.0f - u - v;
                glm::vec3 top = u * tri.A + v * tri.B + w * tri.C;

                GridKey key
                {
                    int(floor(top.x / SupportSpacing)),
                    int(floor(top.z / SupportSpacing))
                };

                if (occupied.contains(key))
                    continue;
                if (!IsPathClearDown(top, tri.id,world))
                    continue;

                glm::vec3 bodyTop;
                glm::vec3 bottom;
                ProjectSinglePoint(top, tri.n, tri.id, world, SupportRadius, bottom, bodyTop);

                glm::vec3 axis = bottom - top;
                if (glm::length(axis) < 1e-6f)
                    continue;
                if (bottom.y <= 0.01f)//only occupied if reached the bed
                {
                    occupied.insert(key);
                    baseClusters[key].push_back(bottom);
                }
                CreateSupportPillar(top, bodyTop, bottom, outSupports);
            }
        }

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
        if (i + 2 < outSupports.size())//enough vertices
        {
            Triangle supportTri;
            supportTri.A = outSupports[i];
            supportTri.B = outSupports[i + 2];//avoid backfaces!
            supportTri.C = outSupports[i + 1];
            supportTri.n = glm::normalize(glm::cross(supportTri.B - supportTri.A, supportTri.C - supportTri.A));//calculate the normal
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
                if (glm::any(glm::isnan(hit))) 
                {
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
        if (glm::any(glm::isnan(bestHit))) 
        {
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
        return false;//hit is behind ray origin

    out_t = t;
    out_hit = rayOrigin + rayDir * t;
    return true;
}

void CreateSupportPillar(const glm::vec3& contactTop, const glm::vec3& bodyTop, const glm::vec3& bot, std::vector<glm::vec3>& outSupports)
{
    float totalHeight = contactTop.y - bot.y;
    if (totalHeight < 1e-4f) return;

    int segments = 12;
    float radius = SupportRadius;

    glm::vec3 horizDir = glm::vec3(bodyTop.x - contactTop.x, 0.0f, bodyTop.z - contactTop.z);
    float offsetDist = glm::length(horizDir);

    if (offsetDist > 1e-4f) {
        horizDir /= offsetDist;
    }
    else {
        horizDir = glm::vec3(1, 0, 0);
    }

    float dropY = std::min(offsetDist * 1.8f, totalHeight * 0.45f);
    float actualOffset = std::min(offsetDist, dropY);

    glm::vec3 P0 = contactTop;
    glm::vec3 P1 = contactTop + horizDir * actualOffset - glm::vec3(0, dropY * 0.6f, 0);
    glm::vec3 P2 = contactTop + horizDir * actualOffset - glm::vec3(0, dropY, 0);
    glm::vec3 PBot = glm::vec3(P2.x, bot.y, P2.z);

    struct Node
    {
        glm::vec3 center;
        float radius;
        glm::vec3 dir;
    };

    std::vector<Node> nodes;

    //Bazier sample
    int numCurveSamples = 6;
    for (int i = 0; i <= numCurveSamples; ++i)
    {
        float t = (float)i / numCurveSamples;
        float invT = 1.0f - t;

        glm::vec3 pos = invT * invT * P0 + 2.0f * invT * t * P1 + t * t * P2;
        glm::vec3 tangent = 2.0f * invT * (P1 - P0) + 2.0f * t * (P2 - P1);

        float tangLen = glm::length(tangent);
        glm::vec3 dir = (tangLen > 1e-5f) ? (tangent / tangLen) : glm::vec3(0, -1, 0);

        float nodeRadius = radius;
        if (t < 0.3f) {
            float tipT = t / 0.3f;
            nodeRadius = glm::mix(radius * 0.35f, radius, tipT);
        }

        nodes.push_back({ pos, nodeRadius, dir });
    }

    //final node in the bed
    nodes.push_back({ PBot, radius, glm::vec3(0, -1, 0) });

    auto addTri = [&](glm::vec3 A, glm::vec3 B, glm::vec3 C)
    {
            outSupports.push_back(A);
            outSupports.push_back(B);
            outSupports.push_back(C);
    };

    std::vector<std::vector<glm::vec3>> rings;

    //initial reference frame
    glm::vec3 prevDir = nodes[0].dir;
    glm::vec3 initialUp = (std::abs(prevDir.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
    glm::vec3 right = glm::normalize(glm::cross(prevDir, initialUp));
    glm::vec3 forward = glm::normalize(glm::cross(prevDir, right));

    for (size_t n = 0; n < nodes.size(); ++n)
    {
        const auto& node = nodes[n];

        if (n > 0)
        {
            glm::vec3 curDir = node.dir;
            glm::vec3 axis = glm::cross(prevDir, curDir);
            float angle = glm::length(axis);

            if (angle > 1e-5f)
            {
                axis = glm::normalize(axis);
                float dotVal = glm::clamp(glm::dot(prevDir, curDir), -1.0f, 1.0f);
                float rotAngle = std::acos(dotVal);

                glm::mat4 rot = glm::rotate(glm::mat4(1.0f), rotAngle, axis);
                right = glm::normalize(glm::vec3(rot * glm::vec4(right, 0.0f)));
                forward = glm::normalize(glm::cross(curDir, right));
            }
            prevDir = curDir;
        }

        std::vector<glm::vec3> ring;
        for (int i = 0; i < segments; ++i)
        {
            float angle = (i / (float)segments) * 2.0f * glm::pi<float>();
            glm::vec3 offset = (std::cos(angle) * right + std::sin(angle) * forward) * node.radius;
            ring.push_back(node.center + offset);
        }
        rings.push_back(ring);
    }

    //connect the rings in the column
    for (size_t r = 0; r < rings.size() - 1; ++r)
    {
        const auto& ringA = rings[r];
        const auto& ringB = rings[r + 1];

        for (int i = 0; i < segments; ++i)
        {
            int next = (i + 1) % segments;

            glm::vec3 t0 = ringA[i];
            glm::vec3 t1 = ringA[next];
            glm::vec3 b0 = ringB[i];
            glm::vec3 b1 = ringB[next];

            addTri(t0, b0, t1);
            addTri(t1, b0, b1);
        }
    }

    //close tip
    const auto& topRing = rings.front();
    for (int i = 0; i < segments; ++i)
    {
        int next = (i + 1) % segments;
        addTri(contactTop, topRing[next], topRing[i]);
    }

    //close the bottom
    const auto& botRing = rings.back();
    for (int i = 0; i < segments; ++i)
    {
        int next = (i + 1) % segments;
        addTri(PBot, botRing[i], botRing[next]);
    }
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
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(TRS)));
        t.n = glm::normalize(normalMatrix * tri.n);
        t.id = tri.id;// preserve the ID

        world.push_back(t);
    }

    return world;
}


//top, tri.n, tri.id, world, SupportRadius, bottom, bodyTop
bool ProjectSinglePoint(const glm::vec3& top, const glm::vec3& normal, int CurrId, const std::vector<Triangle>& world, float supportRadius, glm::vec3& outBottom, glm::vec3& outAdjustedTop)
{
    glm::vec3 rayDir(0, -1, 0);

    //Push outwards through the normal
    glm::vec3 outwardDir = glm::vec3(normal.x, 0.0f, normal.z);
    if (glm::length(outwardDir) > 1e-4f) 
    {
        outwardDir = glm::normalize(outwardDir);
    }
    else//in case its completly vertical
    {
        outwardDir = glm::vec3(1, 0, 0);
    }

    //clear the radius of the support
    float safetyMargin = supportRadius * 1.5f;
    glm::vec3 offsetTop = top + outwardDir * safetyMargin;

    //proyect down
    glm::vec3 origin = offsetTop + normal * 0.02f;
    float bestY = -FLT_MAX;
    bool hit = false;
    const float minPillarHeight = 1.0f;

    for (const Triangle& t : world)
    {
        if (t.id == CurrId) 
            continue;

        float dist;
        glm::vec3 hitPoint;
        if (RayIntersectTriangle(origin, rayDir, t, dist, hitPoint))//if collition with mesh
        {
            float drop = offsetTop.y - hitPoint.y;
            if (drop < minPillarHeight) 
                continue;
            if (glm::dot(t.n, glm::vec3(0, 1, 0)) < 0.3f) 
                continue;

            if (hitPoint.y > bestY)
            {
                bestY = hitPoint.y;
                outBottom = hitPoint;
                hit = true;
            }
        }
    }
    if (!hit)
        outBottom = glm::vec3(offsetTop.x, 0.0f, offsetTop.z); //reach the bed

    //readjust the top
    outAdjustedTop = offsetTop;
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
    glm::vec3 rayOrigin = p + glm::vec3(0, 0.01f, 0);
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
            if (t > 0.01f) 
                return false;
        }
    }
    return true;

}


bool IsPathClearDown(const glm::vec3& top, int CurrId,const std::vector<Triangle>& world)
{
    const float minClearance = 1.0f; //Ignore too close hits
    glm::vec3 origin = top + world[CurrId].n * 0.02f; //Push towards the outside of the model
    glm::vec3 dir(0, -1, 0);

    for (const Triangle& t : world)
    {
        if (t.id == CurrId)
            continue;

        float tHit;
        glm::vec3 hit;
        if (RayIntersectTriangle(origin, dir, t, tHit, hit))
        {
            float drop = top.y - hit.y;
            //if collition with upwards facing face
            if (drop > minClearance && glm::dot(t.n, glm::vec3(0, 1, 0)) > 0.3f)
            {
                return false;//path obstructed
            }
        }
    }

    return true;
}
