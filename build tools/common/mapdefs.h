//
//  mapdefs.h
//  Brushstroke
//
//  Created by Henry Dunn on 8/21/24.
//

#ifndef mapdefs_h
#define mapdefs_h

#include "polygon.h"

#define MAP_MAX_BOUNDS 4096

#define MAX_KEY     32
#define MAX_VALUE   1024

#define NHULLS 4

#define CONTENTS_EMPTY        -1
#define CONTENTS_SOLID        -2
#define CONTENTS_WATER        -3
#define CONTENTS_SLIME        -4
#define CONTENTS_LAVA         -5
#define CONTENTS_SKY          -6
#define CONTENTS_ORIGIN       -7
#define CONTENTS_CLIP         -8
#define CONTENTS_CURRENT_0    -9
#define CONTENTS_CURRENT_90   -10
#define CONTENTS_CURRENT_180  -11
#define CONTENTS_CURRENT_270  -12
#define CONTENTS_CURRENT_UP   -13
#define CONTENTS_CURRENT_DOWN -14
#define CONTENTS_TRANSLUCENT  -15

typedef struct brushdef_t brushdef_t;

typedef struct brushset_t brushset_t;
typedef struct brushsetnode_t brushsetnode_t;
typedef struct brush_t brush_t;
typedef struct surf_t surf_t;
typedef struct surfnode_t surfnode_t;
typedef struct leaf_t leaf_t;
typedef struct splitplane_t splitplane_t;
typedef struct bspnode_t bspnode_t;
typedef struct bspleaf_t bspleaf_t;

typedef struct portalnode_t portalnode_t;

typedef struct
{
    char key[MAX_KEY+1];
    char val[MAX_VALUE+1];
    struct entitypair_t* next, *last;
} entitypair_t;

struct brushdef_t
{
    struct polynode_t* firstp;
    struct polynode_t* lastp;
    planedef_t* firstpl;
    planedef_t* lastpl;
    int nplanes;
    vec3_t bbmin, bbmax;
    brushdef_t* next;
};

typedef struct
{
    entitypair_t* pairs;
    int npairs;
    brushdef_t* firstbrsh;
    brushdef_t* lastbrsh;
    struct entitydef_t *next;
} entitydef_t;

struct brushset_t
{
    surfnode_t* firstsurf;
    vec3_t bbmin, bbmax;
};

struct brushsetnode_t
{
    brushset_t* brushet;
    brushsetnode_t* last, * next;
};

struct surf_t
{
    polynode_t geo;
    vec3_t s, t;
    int sshift, tshift;
    char texname[17];
    splitplane_t* parent;
    vec3_t n;
    float d;
    boolean onplane;
};

struct surfnode_t
{
    surf_t* surf;
    surfnode_t* last, * next;
};

struct leaf_t
{
    int ileaf;
    int contents;
    surfnode_t* surfs;
};

struct splitplane_t
{
    vec3_t n;
    float d;
    leaf_t* leaf; // If not null, node is a leaf
    surfnode_t* surfs;
    surfnode_t* mainsurf;
    surfnode_t* childsurfs[2];
    splitplane_t* children[2];
    portalnode_t* portals;
};

struct bspleaf_t
{
    surfnode_t* surfs;
    int contents;
};

void SurfBB(surf_t* surf, vec3_t* outmin, vec3_t* outmax);
void SurfListBB(surfnode_t* surfs, vec3_t* outmin, vec3_t* outmax);

#endif /* mapdefs_h */
