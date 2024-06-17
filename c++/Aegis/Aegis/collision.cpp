#include "collision.h"

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