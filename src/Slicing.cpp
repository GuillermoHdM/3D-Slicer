#include <unordered_map>
#include "Slicing.h"
using EndpointMap = std::unordered_map<GridKey, std::vector<int>, GridKeyHash>;
GridKey ToGrid(const glm::vec2& p, float tol)
{
    return GridKey{ (int)std::floor(p.x / tol), (int)std::floor(p.y / tol) };
};

std::vector<MeshSlice> GenerateMeshSlices(const std::vector<Triangle>& model, float layerHeight)
{
    std::vector<MeshSlice> slices;
    float yMin = +FLT_MAX;
    float yMax = -FLT_MAX;

    for (const auto& tri : model)
    {
        yMin = std::min({ yMin, tri.A.y, tri.B.y, tri.C.y });
        yMax = std::max({ yMax, tri.A.y, tri.B.y, tri.C.y });
    }

    int numLayers = static_cast<int>(ceil((yMax - yMin) / layerHeight));

    for (int i = 0; i < numLayers; ++i)
    {
        float y = yMin + (i + 0.5f) * layerHeight;
        std::vector<Segment> layerSegments;

        for (const auto& tri : model)
        {
            auto seg = IntersectTriangleWithPlane(tri, y);
            if (seg) layerSegments.push_back(*seg);
        }

        MeshSlice layerContours = ConnectSegments(layerSegments);
        slices.push_back(layerContours);
    }
    return slices;
}

std::optional<Segment> IntersectTriangleWithPlane(const Triangle& tri, float y)
{
    const glm::vec3 verts[3] = { tri.A, tri.B, tri.C };
    if (fabs(tri.A.y - y) < 1e-6f && fabs(tri.B.y - y) < 1e-6f && fabs(tri.C.y - y) < 1e-6f)
    {
        return std::nullopt;
    }
    std::vector<glm::vec2> points;

    for (int i = 0; i < 3; ++i)
    {
        const glm::vec3& a = verts[i];
        const glm::vec3& b = verts[(i + 1) % 3];

        // Si la arista cruza el plano z
        const float eps = 1e-6f;
        float da = a.y - y;
        float db = b.y - y;

        if ((da > 0 && db < 0) || (da < 0 && db > 0))
        {
            float t = da / (da - db);
            glm::vec3 p = a + t * (b - a);
            points.push_back(glm::vec2(p.x, p.z));
        }
    }

    if (points.size() == 2)
        return Segment{ points[0], points[1] };
    else
        return std::nullopt;
}

MeshSlice ConnectSegments(const std::vector<Segment>& segments, float tol)
{
    MeshSlice contours;

    EndpointMap map;

    //Give index to endpoints
    for (int i = 0; i < (int)segments.size(); ++i)
    {
        map[ToGrid(segments[i].A, tol)].push_back(i);
        map[ToGrid(segments[i].B, tol)].push_back(i);
    }

    std::vector<bool> used(segments.size(), false);

    //build the contours
    for (int i = 0; i < (int)segments.size(); ++i)
    {
        if (used[i]) continue;

        std::vector<glm::vec2> contour;

        glm::vec2 start = segments[i].A;
        glm::vec2 current = segments[i].B;

        contour.push_back(start);
        contour.push_back(current);

        used[i] = true;

        bool closed = false;

        while (!closed)
        {
            GridKey key = ToGrid(current, tol);

            auto it = map.find(key);
            if (it == map.end()) break;

            bool extended = false;

            for (int segIdx : it->second)
            {
                if (used[segIdx]) continue;

                const auto& s = segments[segIdx];

                if (glm::length(s.A - current) < tol)
                {
                    current = s.B;
                }
                else if (glm::length(s.B - current) < tol)
                {
                    current = s.A;
                }
                else
                {
                    continue;
                }

                contour.push_back(current);
                used[segIdx] = true;
                extended = true;
                break;
            }

            if (!extended)
                break;

            if (glm::length(contour.front() - current) < tol)
            {
                contour.back() = contour.front();
                closed = true;
            }
        }

        if (contour.size() > 2)
            contours.push_back(contour);
    }

    return contours;
}

