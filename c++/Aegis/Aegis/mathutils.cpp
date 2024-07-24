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

vec3_t ReflectVector3(vec3_t d, vec3_t n)
{
    float dot = DotProduct(d, n);

    return d - (n * 2 * dot);
}

vec3_t Vector3Lerp(vec3_t a, vec3_t b, float t)
{
    vec3_t v;
    v.x = a.x + (b.x - a.x) * t;
    v.y = a.y + (b.y - a.y) * t;
    v.z = a.z + (b.z - a.z) * t;
    return v;
}

vec2_t Vector2Lerp(vec2_t a, vec2_t b, float t)
{
    vec2_t v;
    v.x = a.x + (b.x - a.x) * t;
    v.y = a.y + (b.y - a.y) * t;
    return v;
}

vec2_t Vector2Bezier(vec2_t p0, vec2_t p1, vec2_t p2, float t)
{
    vec2_t a = Vector2Lerp(p0, p1, t);
    vec2_t b = Vector2Lerp(p1, p2, t);
    return Vector2Lerp(a, b, t);
}

float Lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

float maxf(float a, float b)
{
    if (a > b) return a;
    return b;
}

float minf(float a, float b)
{
    if (a > b) return b;
    return a;
}