#include "Slicing.h"

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

    int numLayers = static_cast<int>((yMax - yMin) / layerHeight) + 1;

    for (int i = 0; i < numLayers; ++i)
    {
        float y = yMin + i * layerHeight;
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
        bool above = a.y >= y;
        bool below = b.y >= y;
        if (above != below)
        {
            float t = (y - a.y) / (b.y - a.y);
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
    std::vector<bool> used(segments.size(), false);

    for (size_t i = 0; i < segments.size(); ++i) {
        if (used[i]) continue;

        std::vector<glm::vec2> contour;
        contour.push_back(segments[i].A);
        contour.push_back(segments[i].B);
        used[i] = true;

        bool extended = true;
        while (extended) {
            extended = false;
            for (size_t j = 0; j < segments.size(); ++j) {
                if (used[j]) continue;
                auto& s = segments[j];

                if (glm::length(contour.back() - s.A) < tol) {
                    contour.push_back(s.B);
                    used[j] = true;
                    extended = true;
                }
                else if (glm::length(contour.back() - s.B) < tol) {
                    contour.push_back(s.A);
                    used[j] = true;
                    extended = true;
                }
            }
        }

        // cerrar bucle si hace falta
        if (glm::length(contour.front() - contour.back()) < tol)
            contour.back() = contour.front();

        contours.push_back(contour);
    }

    return contours;
}