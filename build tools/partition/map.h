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
typedef struct brushetnode_t brushetnode_t;
typedef struct brush_t brush_t;
typedef struct surf_t surf_t;
typedef struct surfnode_t surfnode_t;
typedef struct splitplane_t splitplane_t;
typedef struct bspnode_t bspnode_t;
typedef struct bspleaf_t bspleaf_t;

struct brushset_t
{
    brush_t *firstbr;
    int nbr;
    vec3_t bbmin, bbmax;
};

struct brushsetnode_t
{
    brushset_t *brushet;
    brushetnode_t *last, *next;
};

struct brush_t
{
    surfnode_t *firstsurf;
    int nsurfs, contents;
    vec3_t bbmin, bbmax;
    brush_t *last, *next;
};

struct surf_t
{
    polynode_t geo;
    vec3_t s, t;
    int sshift, tshift;
    char texname[17];
    splitplane_t *parent;
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
FILE *gfiles[NHULLS];
struct brushsetnode_t *brushsets[NHULLS];

void LoadBrushSets(char* file);
surf_t FindIdealSplitSurf(surfnode_t *surfs);

#endif /* map_h */
