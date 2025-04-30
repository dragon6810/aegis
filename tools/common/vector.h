#ifndef _VECTOR_H
#define _VECTOR_H

#include <stdio.h>

typedef float vec3_t[3];

static vec3_t vec3_origin = {0, 0, 0};

typedef struct poly_s
{
    int npoints;
    vec3_t points[0];
} poly_t;

void VectorCopy(vec3_t dest, vec3_t vec);
void VectorAdd(vec3_t dest, vec3_t a, vec3_t b);
void VectorSubtract(vec3_t dest, vec3_t a, vec3_t b);
void VectorScale(vec3_t dest, vec3_t v, float s);
void VectorScaleAdd(vec3_t dest, vec3_t a, vec3_t b, float s);
void VectorNormalize(vec3_t dest, vec3_t v);
float VectorDot(vec3_t a, vec3_t b);
void VectorCross(vec3_t dest, vec3_t a, vec3_t b);

poly_t* AllocPoly(int npoints);
poly_t* CopyPoly(poly_t* poly);
poly_t* PolyForPlane(vec3_t n, float d);
poly_t* CutPoly(poly_t* poly, vec3_t n, float d, int side);
void PrintPoly(FILE* out, poly_t* poly);

#endif