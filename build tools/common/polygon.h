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

typedef struct planedef_t planedef_t;
typedef struct vnode_t vnode_t;

struct vnode_t
{
    vec3_t val;
    float a;
    struct vnode_t *next, *last;
};

typedef struct
{
    vnode_t* first; // Cycically doubley linked list
    vec3_t normal;
    planedef_t* pl;
} polynode_t;

struct planedef_t
{
    vec3_t n;
    float d;
    struct planedef_t* next;
};

vnode_t* AllocVert();
polynode_t* AllocPoly();

// Generates a full map poly from a plane. Vertices are wound counter clockwise.
void HungryPoly(polynode_t* poly, vec3_t n, float d);

// Winds a convex polygon to be counter clockwise
void WindPoly(polynode_t* poly);

// Clips a convex polygon to only be behind a plane
void ClipPoly(polynode_t* poly, vec3_t n, float d);

void ClipVert(vnode_t* v, polynode_t* p);

int CompNodes(const void* a, const void* b);

#endif /* polygon_h */
