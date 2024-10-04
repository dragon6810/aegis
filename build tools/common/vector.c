#include "vector.h"

#include <math.h>

void VectorCopy(vec3_t dest, vec3_t src)
{
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
}

void VectorAdd(vec3_t dest, vec3_t a, vec3_t b)
{
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
}

void VectorSubtract(vec3_t dest, vec3_t a, vec3_t b)
{
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
    dest[2] = a[2] - b[2];
}

void VectorMultiply(vec3_t dest, vec3_t a, float b)
{
    dest[0] = a[0] * b;
    dest[1] = a[1] * b;
    dest[2] = a[2] * b;
}

void VectorDivide(vec3_t dest, vec3_t a, float b)
{
    VectorMultiply(dest, a, 1.0 / b);
}

void VectorNormalize(vec3_t dest, vec3_t src)
{
    VectorDivide(dest, src, VectorLength(src));
}

float VectorLength(vec3_t src)
{
    return sqrtf(VectorLengthSqr(src));
}

float VectorLengthSqr(vec3_t src)
{
    return VectorDot(src, src);
}

float VectorDot(vec3_t a, vec3_t b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void VectorCross(vec3_t dest, vec3_t a, vec3_t b)
{
    dest[0] = a[1] * b[2] - a[2] * b[1];
    dest[1] = a[2] * b[0] - a[0] * b[2];
    dest[2] = a[0] * b[1] - a[1] * b[0];
}

boolean VectorComp(vec3_t a, vec3_t b)
{
    if(a[0] != b[0]) return false;
    if(a[1] != b[1]) return false;
    if(a[2] != b[2]) return false;
    return true;
}
