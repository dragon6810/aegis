//
//  map.h
//  Partition
//
//  Created by Henry Dunn on 9/18/24.
//

#ifndef map_h
#define map_h

#include "polygon.h"
#include <string.h>

#define NHULLS 4

typedef struct brushset_t brushset_t;
typedef struct brushsetnode_t brushsetnode_t;
typedef struct brush_t brush_t;
typedef struct surf_t surf_t;
typedef struct surfnode_t surfnode_t;
typedef struct splitplane_t splitplane_t;
typedef struct bspnode_t bspnode_t;
typedef struct bspleaf_t bspleaf_t;

struct brushset_t
{
    surfnode_t *firstsurf;
    vec3_t bbmin, bbmax;
};

struct brushsetnode_t
{
    brushset_t *brushet;
    brushsetnode_t *last, *next;
};

struct surf_t
{
    polynode_t geo;
    vec3_t s, t;
    int sshift, tshift;
    char texname[17];
    splitplane_t *parent;
    vec3_t n;
    float d;
    boolean onplane;
};

struct surfnode_t
{
    surf_t *surf;
    surfnode_t *last, *next;
};

struct splitplane_t
{
    vec3_t n;
    float d;
    surfnode_t *surfs;
    surfnode_t *childsurfs[2];
    splitplane_t *children[2];
};

struct bspnode_t
{
    splitplane_t *plane;
    bspleaf_t *leaf; // If not a nullptr, the node is a leaf.
    bspnode_t *front, *back;
};

struct bspleaf_t
{
    surfnode_t *surfs;
    int contents;
};

char *filename;
FILE* gfiles[NHULLS];
brushsetnode_t *brushsets[NHULLS];
splitplane_t rootnode, outsidenode;

void LoadBrushSets(char* file);
void ProcessWorld();
splitplane_t MakeSplitNode(surfnode_t *surfs);
void CutWorld_r(splitplane_t* parent);
surfnode_t* FindIdealSplitSurf(surfnode_t *surfs);

// -1 for back, 0 for on, 1 for front, 2 for both
int GetSurfSide(surf_t* surf, vec3_t n, float d);
surf_t* CopySurf(surf_t* surf);

#endif /* map_h */
