#pragma once

#include <vector>
#include <optional>

#include "Vector.h"
#include "Matrix.h"

class PolyMath
{
public:
    static std::vector<Vector3> BaseWindingForPlane(Vector3 n, float d, float maxrange=8192);
    static bool PlaneCrosses(std::vector<Vector3> points, Vector3 n, float d);
    static std::vector<Vector3> ClipToPlane(std::vector<Vector3> points, Vector3 n, float d, bool front);
    static Vector3 FindCenter(std::vector<Vector3> points);
    static Vector3 FindNormal(std::vector<Vector3> points);
    static std::optional<Vector3> SegmentIntersects(std::vector<Vector3> points, Vector3 a, Vector3 b);
    static Matrix3x3 PlaneProjection(Vector3 n);
    static std::optional<Vector3> SegmentPlane(Vector3 n, float d, Vector3 a, Vector3 b);
    static bool PointIn2d(std::vector<Vector2> points, Vector2 p);
};