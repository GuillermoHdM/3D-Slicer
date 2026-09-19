#include <unordered_map>
#include "Slicing.h"
using EndpointMap = std::unordered_map<GridKey, std::vector<int>, GridKeyHash>;
GridKey ToGrid(const glm::vec2& p, float tol)
{
    return GridKey{ (int)std::floor(p.x / tol), (int)std::floor(p.y / tol) };
};

std::vector<MeshSlice> GenerateMeshSlices(const std::vector<Triangle>& model, float layerHeight, glm::mat4 trs)
{
    std::vector<MeshSlice> slices;
    std::vector<Triangle> transformed;
    transformed.reserve(model.size());
    float yMin = +FLT_MAX;
    float yMax = -FLT_MAX;

    for (const auto& tri : model)
    {
        Triangle T;
        T.A = glm::vec3(trs * glm::vec4(tri.A, 1.0f));
        T.B = glm::vec3(trs * glm::vec4(tri.B, 1.0f));
        T.C = glm::vec3(trs * glm::vec4(tri.C, 1.0f));
        transformed.push_back(T);
    }
    for (const auto& tri : transformed)
    {
        yMin = std::min({ yMin, tri.A.y, tri.B.y, tri.C.y });
        yMax = std::max({ yMax, tri.A.y, tri.B.y, tri.C.y });
    }

    int numLayers = static_cast<int>(ceil((yMax - yMin) / layerHeight));

    for (int i = 0; i < numLayers; ++i)
    {
        float y = yMin + (i + 0.5f) * layerHeight + 1e-5f;
        std::vector<Segment> layerSegments;

        for (const auto& tri : transformed)
        {
            float triYMin = std::min({ tri.A.y, tri.B.y, tri.C.y });
            float triYMax = std::max({ tri.A.y, tri.B.y, tri.C.y });
            if (y < triYMin || y > triYMax)
                continue;
            auto seg = IntersectTriangleWithPlane(tri, y);
            if (seg) 
                layerSegments.push_back(*seg);
        }

        MeshSlice layerContours = ConnectSegments(layerSegments);
        slices.push_back(layerContours);
    }
    return slices;
}

std::optional<Segment> IntersectTriangleWithPlane(const Triangle& tri, float y)
{
    const glm::vec3 verts[3] = { tri.A, tri.B, tri.C };

    //Vertex position on y plane
    float d[3] = { verts[0].y - y, verts[1].y - y, verts[2].y - y };

    //All on the same side
    if ((d[0] > 1e-6f && d[1] > 1e-6f && d[2] > 1e-6f) ||
        (d[0] < -1e-6f && d[1] < -1e-6f && d[2] < -1e-6f))
    {
        return std::nullopt;
    }

    std::vector<glm::vec2> points;

    for (int i = 0; i < 3; ++i)
    {
        int j = (i + 1) % 3;
        float da = d[i];
        float db = d[j];

        //Edge crossing
        if ((da >= 0.0f && db < 0.0f) || (da < 0.0f && db >= 0.0f))
        {
            float t = da / (da - db);
            glm::vec3 p = verts[i] + t * (verts[j] - verts[i]);

            glm::vec2 pt2D(p.x, p.z);

            //Avoid duplicated vertices
            if (points.empty() || glm::distance(points.back(), pt2D) > 1e-5f)
            {
                points.push_back(pt2D);
            }
        }
    }

    if (points.size() == 2)
        return Segment{ points[0], points[1] };

    return std::nullopt;
}

MeshSlice ConnectSegments(const std::vector<Segment>& segments, float tol)
{
    MeshSlice contours;
    if (segments.empty()) return contours;

    EndpointMap map;

    //Index final points on mesh
    for (int i = 0; i < (int)segments.size(); ++i)
    {
        map[ToGrid(segments[i].A, tol)].push_back(i);
        map[ToGrid(segments[i].B, tol)].push_back(i);
    }

    std::vector<bool> used(segments.size(), false);

    for (int i = 0; i < (int)segments.size(); ++i)
    {
        if (used[i])
            continue;

        std::vector<glm::vec2> contour;
        glm::vec2 start = segments[i].A;
        glm::vec2 current = segments[i].B;

        contour.push_back(start);
        contour.push_back(current);
        used[i] = true;

        bool closed = false;

        while (!closed)
        {
            GridKey centerKey = ToGrid(current, tol);
            bool extended = false;

            //search the 9 neightbour tiles
            //This avoids the contour to break if the point falls on the edge of the mesh
            for (int dx = -1; dx <= 1 && !extended; ++dx)
            {
                for (int dy = -1; dy <= 1 && !extended; ++dy)
                {
                    GridKey neighborKey = { centerKey.x + dx, centerKey.z + dy };
                    auto it = map.find(neighborKey);
                    if (it == map.end()) continue;

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
                }
            }
            if (!extended) 
                break;

            //Close the loop
            if (glm::length(contour.front() - current) < tol)
            {
                contour.back() = contour.front();
                closed = true;
            }
        }

        if (contour.size() > 2)
        {
            //if last is same as first avoid generating degenerate cases
            if (contour.front() == contour.back())
            {
                contour.pop_back();
            }
            contours.push_back(contour);
        }
    }

    return contours;
}

