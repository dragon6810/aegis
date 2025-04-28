#ifndef _VECTOR_H
#define _VECTOR_H

typedef float vec3_t[3];

static vec3_t vec3_origin = {0, 0, 0};

typedef struct poly_s
{
    int npoints;
    vec3_t points[0];
} poly_t;

void VectorCopy(vec3_t dest, vec3_t vec);

poly_t* AllocPoly(int npoints);

#endif