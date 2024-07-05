#include "collision.h"

#include "mathutils.h"

std::vector<vec3_t> ClipPolygon(std::vector<vec3_t> poly, vec3_t n, float d)
{
    std::vector<vec3_t> clippedPolygon;

    for (size_t i = 0; i < poly.size(); ++i)
    {
        vec3_t curpoint = poly[i];
        vec3_t nextpoint = poly[(i + 1) % poly.size()];

        float curd = n.x * curpoint.x + n.y * curpoint.y + n.z * curpoint.z - d;
        float nextd = n.x * nextpoint.x + n.y * nextpoint.y + n.z * nextpoint.z - d;

        bool v1Inside = curd<= 0;
        bool v2Inside = nextd <= 0;

        if (v1Inside)
            clippedPolygon.push_back(curpoint);

        if (v1Inside != v2Inside) 
        {
            float t = curd / (curd - nextd);
            vec3_t intersection = 
            {
                curpoint.x + t * (nextpoint.x - curpoint.x),
                curpoint.y + t * (nextpoint.y - curpoint.y),
                curpoint.z + t * (nextpoint.z - curpoint.z)
            };
            clippedPolygon.push_back(intersection);
        }
    }

    return clippedPolygon;
}

vec3_t PlaneIntersection(vec3_t a, vec3_t b, vec3_t n, float d)
{
    float N = n.x * (b.x - a.x) + n.y * (b.y - a.y) + n.z * (b.z - a.z);
    float d1 = n.x * a.x + n.y * a.y + n.z * a.z + d;
    float t = -d1 / N;
    return { (1 - t) * a.x + t * b.x, (1 - t) * a.y + t * b.y, (1 - t) * a.z + t * b.z };
}

bool IsPointInPolygon(vec3_t point, const std::vector<vec3_t>& polygon, vec3_t normal)
{
    vec3_t right = CrossProduct(normal, { 0.0, 0.0, 1.0 });
    vec3_t up = CrossProduct(normal, right);
    right = CrossProduct(up, normal);

    std::vector<vec2_t> transpoly;
    for (int i = 0; i < polygon.size(); i++)
    {
        vec3_t pos = polygon[i] - point;
        vec2_t transpos = { DotProduct(pos, right), DotProduct(pos, up) };
        transpoly.push_back(transpos);
    }

    int numintersections = 0;
    for (int i = 0; i < transpoly.size(); i++)
    {
        vec2_t p1 = transpoly[i];
        vec2_t p2 = transpoly[(i + 1) % transpoly.size()];

        if (p1.y * p2.y > 0)
            continue;

        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;

        if (dx == 0)
        {
            if (p1.x > 0)
                numintersections++;

            continue;
        }

        float m = dy / dx;
        float b = p1.y - m * p1.x;

        if (m == 0)
            continue;

        float xintercept = -b / m;

        if (xintercept >= 0)
            numintersections++;
    }

    return numintersections & 1;
}

std::vector<vec3_t> BoxFace(vec3_t bmin, vec3_t bmax, std::vector<vec3_t> face)
{
    vec3_t planeNormals[6] = 
    {
        { 1.0,  0.0,  0.0}, {-1.0,  0.0,  0.0}, // x planes
        { 0.0,  1.0,  0.0}, { 0.0, -1.0,  0.0}, // y planes
        { 0.0,  0.0,  1.0}, { 0.0,  0.0, -1.0}  // z planes
    };
    float planeDistances[6] = 
    {
        bmax.x, -bmin.x,
        bmax.y, -bmin.y,
        bmax.z, -bmin.z
    };

    std::vector<vec3_t> resultPolygon = face;
    for (int i = 0; i < 6; ++i)
        resultPolygon = ClipPolygon(resultPolygon, planeNormals[i], planeDistances[i]);

    return resultPolygon;
}

float TriangleArea(vec2_t p0, vec2_t p1, vec2_t p2)
{
    return abs((p0.x * (p1.y - p2.y) + p1.x * (p2.y - p0.y) + p2.x * (p0.y - p1.y)) / 2.0);
}

bool PointInTriangle(vec2_t p0, vec2_t p1, vec2_t p2, vec2_t p)
{
    const float epsilon = 0.001;

    float a;
    float a1;
    float a2;
    float a3;

    a = TriangleArea(p0, p1, p2);
    a1 = TriangleArea(p, p0, p1);
    a2 = TriangleArea(p, p0, p2);
    a3 = TriangleArea(p, p1, p2);

    // Check if the sum of the areas of the triangle point p forms with the vertices is (roughly) equal to the area of the triangle.
    // TODO: Recede the vertices by epsilon / 2 to account for epsilon growing the triangle a little
    return abs(a - a1 - a2 - a3) < epsilon;
}

float PolygonDirection(std::vector<vec2_t> points)
{
    float area = 0.0;
    for (int i = 0; i < points.size() - 1; i++)
        area += points[i].x * points[i + 1].y - points[i].y * points[i + 1].x;

    return area * 0.5;
}

bool PointInPolygon(std::vector<vec2_t> points, vec2_t p)
{
    uint32_t ncollisions = 0;

    for (int i = 0; i < points.size(); i++)
    {
        vec2_t p0 = points[i] - p;
        vec2_t p1 = points[(i + 1) % points.size()] - p;

        if (SegXIntercept(p0, p1))
            ncollisions++;
    }

    return ncollisions & 1;
}

bool SegXIntercept(vec2_t p0, vec2_t p1)
{
    float temp;
    return SegXIntercept(p0, p1, &temp);
}

bool SegXIntercept(vec2_t p0, vec2_t p1, float* x)
{
    if (p0.y * p1.y > 0) // Both points are on the same side of the x-axis
        return false;

    if (p0.x < 0 && p1.x < 0) // Both points are to the left of the y-axis
        return false;

    if (p0.x == p1.x) // Vertical line?
    {
        // We know it's to the right from earlier checks, so collide.
        *x = p0.x;
        return true;
    }

    if (p0.y == p1.y) // Horizontal line?
    {
        // We know it's along the x axis and to the right from earlier checks, so collide.
        *x = 0;
        return true;
    }

    float rise = p1.y - p0.y;
    float run = p1.x - p0.x;
    float slope = rise / run;
    float b = -slope * p0.x + p0.y;
    float xintercept = -b / slope;

    if (xintercept >= 0) // If the x-intercept of the line segment is to the right or touching the point, collide. (We've already checked the range).
    {
        *x = xintercept;
        return true;
    }

    return false;
}