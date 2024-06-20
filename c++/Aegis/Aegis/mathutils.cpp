#include "mathutils.h"

#include <math.h>

vec3_t NormalizeVector3(vec3_t v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return { v.x / len, v.y / len, v.z / len };
}

float Vector3Length(vec3_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec3_t CrossProduct(vec3_t a, vec3_t b)
{
    float x = a.y * b.z - a.z * b.y;
    float y = a.z * b.x - a.x * b.z;
    float z = a.x * b.y - a.y * b.x;

    if ((x * x + y * y + z * z) < 0.01)
        x = 1;

    return { x, y, z };
}

float DotProduct(vec3_t a, vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}