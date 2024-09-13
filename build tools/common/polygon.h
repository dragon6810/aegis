//
//  polygon.h
//  Brushstroke
//
//  Created by Henry Dunn on 8/21/24.
//

#ifndef polygon_h
#define polygon_h

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"

#define ON_EPSILON 0.1

typedef struct planedef_t planedef_t;
typedef struct polynode_t polynode_t;
typedef struct vnode_t vnode_t;

struct vnode_t
{
    vec3_t val;
    float a;
    struct vnode_t *next, *last;
};

struct polynode_t
{
    vnode_t* first; // Cycically doubley linked list
    vec3_t normal;
    planedef_t* pl;
    polynode_t* next;
    polynode_t* last;
};

struct planedef_t
{
    vec3_t n;
    float d;
    char texname[17];
    vec3_t shat, that; // It's not the past tense of shit and that its s-hat and t-hat because they're basis vectors
    float sshift, tshift;
    struct planedef_t* next;
};

vnode_t* AllocVert();
polynode_t* AllocPoly();

// Generates a full map poly from a plane. Vertices are wound counter clockwise.
void HungryPoly(polynode_t* poly, vec3_t n, float d);

// Winds a convex polygon to be counter clockwise
void WindPoly(polynode_t* poly);

// Clips a cc-wound convex polygon to only be behind a plane.
// Side=0? clip to back
// Side=1? clip to front
void ClipPoly(polynode_t* poly, vec3_t n, float d, int side);
// Slices a cc-wound convex polygon into to along a plane
boolean SlicePoly(polynode_t* poly, vec3_t n, float d);

boolean PolyInsidePlane(polynode_t* poly, vec3_t n, float d);

polynode_t *CopyPoly(polynode_t* p);

void ClipVert(vnode_t* v, polynode_t* p);

int CompNodes(const void* a, const void* b);

#endif /* polygon_h */
