#include <vector.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <std/assert/assert.h>

void VectorCopy(vec3_t dest, vec3_t vec)
{
    int i;

    for(i=0; i<3; i++)
        dest[i] = vec[i];
}

void VectorAdd(vec3_t dest, vec3_t a, vec3_t b)
{
    int i;

    for(i=0; i<3; i++)
        dest[i] = a[i] + b[i];
}

void VectorSubtract(vec3_t dest, vec3_t a, vec3_t b)
{
    int i;

    for(i=0; i<3; i++)
        dest[i] = a[i] - b[i];
}

void VectorScale(vec3_t dest, vec3_t v, float s)
{
    int i;

    for(i=0; i<3; i++)
        dest[i] = v[i] * s;
}

void VectorScaleAdd(vec3_t dest, vec3_t a, vec3_t b, float s)
{
    int i;

    for(i=0; i<3; i++)
        dest[i] = a[i] + b[i] * s;
}

void VectorNormalize(vec3_t dest, vec3_t v)
{
    int i;
    float len;

    for(i=0, len=0; i<3; i++)
        len += v[i] * v[i];
    len = sqrtf(len);

    for(i=0; i<3; i++)
        dest[i] = v[i] / len;
}

float VectorDot(vec3_t a, vec3_t b)
{
    int i;
    float dot;

    for(i=0, dot=0; i<3; i++)
        dot += a[i] * b[i];

    return dot;
}

void VectorCross(vec3_t dest, vec3_t a, vec3_t b)
{
    vec3_t v;

    v[0] = (a[1] * b[2]) - (a[2] * b[1]);
    v[1] = (a[2] * b[0]) - (a[0] * b[2]);
    v[2] = (a[0] * b[1]) - (a[1] * b[0]);

    VectorCopy(dest, v);
}

poly_t* AllocPoly(int npoints)
{
    poly_t* poly;

    poly = calloc(1, sizeof(poly_t) + sizeof(vec3_t) * npoints);
    poly->npoints = npoints;

    return poly;
}

poly_t* CopyPoly(poly_t* poly)
{
    poly_t *new;

    assert(poly);

    new = AllocPoly(poly->npoints);
    memcpy(new->points, poly->points, sizeof(vec3_t) * new->npoints);

    return new;
}

poly_t* PolyForPlane(vec3_t n, float d)
{
    const float bound = 8192;

    int i;

    int mainaxis;
    float maxaxis;
    vec3_t basis[2], center;
    poly_t *poly;

    mainaxis = -1;
    maxaxis = 0;
    for(i=0; i<3; i++)
    {
        if(fabsf(n[i]) > maxaxis)
        {
            mainaxis = i;
            maxaxis = fabsf(n[i]);
        }
    }
    if(mainaxis == -1)
        return NULL;

    VectorCopy(basis[1], vec3_origin);
    switch(mainaxis)
    {
    case 0:
    case 1:
        basis[1][2] = 1;
        break;
    case 2:
        basis[1][1] = 1;
    }

    VectorCross(basis[0], basis[1], n);
    VectorCross(basis[1], n, basis[0]);
    VectorScale(center, n, d);
    for(i=0; i<2; i++)
        VectorScale(basis[i], basis[i], bound);

    poly = AllocPoly(4);
    for(i=0; i<4; i++)
    {
        VectorCopy(poly->points[i], center);

        switch(i)
        {
        case 0:
        case 3:
            VectorAdd(poly->points[i], poly->points[i], basis[0]);
            break;
        case 1:
        case 2:
            VectorSubtract(poly->points[i], poly->points[i], basis[0]);
            break;
        }

        switch(i)
        {
        case 0:
        case 1:
            VectorAdd(poly->points[i], poly->points[i], basis[1]);
            break;
        case 2:
        case 3:
            VectorSubtract(poly->points[i], poly->points[i], basis[1]);
            break;
        }
    }

    return poly;
}

poly_t* CutPoly(poly_t* poly, vec3_t n, float d, int side)
{
    const float epsilon = 0.01;
    const int front = 2, on = 1, back = 0;

    int i;

    poly_t *newpoly;
    vec3_t pln;
    float pld;
    float dists[poly->npoints+1];
    int sides[poly->npoints+1];
    int sidecounts[3] = {0, 0, 0};
    vec3_t p2, o, r, p;
    float t;

    assert(poly);

    VectorCopy(pln, n);
    pld = d;

    if(side)
    {
        VectorScale(pln, pln, -1);
        pld = -pld;
    }

    for(i=0; i<poly->npoints; i++)
    {
        dists[i] = VectorDot(poly->points[i], pln) - pld;
        if(dists[i] > epsilon)
            sides[i] = front;
        else if(dists[i] < -epsilon)
            sides[i] = back;
        else
            sides[i] = on;

        sidecounts[sides[i]]++;
    }
    dists[i] = dists[0];
    sides[i] = sides[0];

    if(!sidecounts[front])
        return CopyPoly(poly);
    if(!sidecounts[back])
        return NULL;

    newpoly = AllocPoly(sidecounts[0] + 2);
    newpoly->npoints = 0;
    for(i=0; i<poly->npoints; i++)
    {
        switch(sides[i])
        {
        case on:
        case back:
            VectorCopy(newpoly->points[newpoly->npoints++], poly->points[i]);
            break;
        default:
            break;
        }

        if(!(sides[i] == back && sides[i+1] == front) &&
           !(sides[i] == front && sides[i+1] == back))
            continue;

        // This edge crosses the plane

        VectorCopy(o, poly->points[i]);
        VectorCopy(p2, poly->points[(i+1)%poly->npoints]);
        VectorSubtract(r, p2, o);
        VectorNormalize(r, r);
        VectorScale(p, pln, pld);

        VectorSubtract(p, p, o);
        t = VectorDot(p, pln) / VectorDot(r, pln);
        VectorScaleAdd(p, o, r, t);
        VectorCopy(newpoly->points[newpoly->npoints++], p);
    }

    return newpoly;
}

bool PolyOnPlane(poly_t* poly, vec3_t n, float d)
{
    const float epsilon = 0.01;

    int i;

    assert(poly);

    for(i=0; i<poly->npoints; i++)
        if(fabsf(VectorDot(poly->points[i], n) - d) > epsilon)
            return false;

    return true;
}

void PrintPoly(FILE* out, poly_t* poly)
{
    int i;

    assert(out);
    assert(poly);

    for(i=0; i<poly->npoints; i++)
    {
        fprintf(out, "( %f %f %f )", poly->points[i][0], poly->points[i][1], poly->points[i][2]);
        if(i < poly->npoints - 1)
            fprintf(out, " ");
    }
}
