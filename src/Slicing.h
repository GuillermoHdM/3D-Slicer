#pragma once
#include <vector>
#include <algorithm>
#include <optional>
#include "OpenGl.hpp"

using MeshSlice = std::vector<std::vector<glm::vec2>>;


std::vector<MeshSlice> GenerateMeshSlices(const std::vector<Triangle>& model, float layerHeight);
std::optional<Segment> IntersectTriangleWithPlane(const Triangle& tri, float z);
MeshSlice ConnectSegments(const std::vector<Segment>& segments, float tol = 1e-4f);