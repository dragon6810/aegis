#pragma once

#include <vector>

#include "Vector.h"

class PolyMath
{
public:
    static std::vector<Vector3> BaseWindingForPlane(Vector3 n, float d, float maxrange=8192);
    static bool PlaneCrosses(std::vector<Vector3> points, Vector3 n, float d);
    static std::vector<Vector3> ClipToPlane(std::vector<Vector3> points, Vector3 n, float d, bool front);
    static Vector3 FindCenter(std::vector<Vector3> points);
};