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