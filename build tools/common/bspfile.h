//
//  bspfile.h
//  Partition
//
//  Created by Henry Dunn on 10/4/24.
//

#ifndef bspfile_h
#define bspfile_h

#include <stdint.h>

#include "vector.h"

#define MAX_MAP_HULLS        4

#define MAX_MAP_MODELS       400
#define MAX_MAP_BRUSHES      4096
#define MAX_MAP_ENTITIES     1024
#define MAX_MAP_ENTSTRING    (128*1024)

#define MAX_MAP_PLANES       32767
#define MAX_MAP_NODES        32767
#define MAX_MAP_CLIPNODES    32767
#define MAX_MAP_LEAFS        8192
#define MAX_MAP_VERTS        65535
#define MAX_MAP_FACES        65535
#define MAX_MAP_MARKSURFACES 65535
#define MAX_MAP_TEXINFO      8192
#define MAX_MAP_EDGES        256000
#define MAX_MAP_SURFEDGES    512000
#define MAX_MAP_TEXTURES     512
#define MAX_MAP_MIPTEX       0x200000
#define MAX_MAP_LIGHTING     0x200000
#define MAX_MAP_VISIBILITY   0x200000

#define MAX_MAP_PORTALS     65536

#define PLANE_X 0     // Plane is perpendicular to given axis
#define PLANE_Y 1
#define PLANE_Z 2
#define PLANE_ANYX 3  // Non-axial plane is snapped to the nearest
#define PLANE_ANYY 4
#define PLANE_ANYZ 5

typedef struct
{
    vec3_t n;
    float d;
    int type;
} bspfplane_t;

typedef struct
{
    char name[16];
    uint32_t width, height;
    uint32_t offsets[4]; // Should be 0
} bspfmiptex_t;

typedef struct
{
    uint32_t plane;
    short children[2];
    short min[3], max[3];
    uint16_t firstface, nfaces;
} bspfnode_t;

typedef struct
{
    vec3_t s;
    float sshift;
    vec3_t t;
    float tshift;
    uint32_t miptex;
    uint32_t flags;
} bspftexinfo_t;

typedef struct
{
    uint16_t plane;
    uint16_t planeside;
    uint32_t firstedge;
    uint16_t nedges;
    uint16_t texinfo;
    uint8_t styles[4];
    int32_t lightmapoffs;
} bspfface_t;

typedef struct
{
    int32_t plane;
    int16_t children[2];
} bspfclipnode_t;

typedef struct
{
    int32_t contents;
    int32_t visoffset;
    int16_t mins[3], maxs[3];
    uint16_t firstmarksurf, nmarksurfs;
    uint8_t ambientlevels[4];
} bspfleaf_t;

typedef uint16_t bspfedge[2];

typedef struct
{
    float min[3], max[3];
    vec3_t origin;
    int32_t rootnodes[MAX_MAP_HULLS];
    int32_t nvisleafs;                // Likely quake remnant, maybe remove in the future
    int32_t firstface, nfaces;
} bspmodel_t;

// This struct is a whopping 15mb, maybe move things into a dynamic struct in the future
// Why didn't I do this in C++ I miss vectors :(
typedef struct
{
    char* entitydata;
    bspfplane_t planes[MAX_MAP_PLANES];
    int nplanes;
    bspfmiptex_t textures[MAX_MAP_TEXTURES];
    int ntextures;
    vec3_t vertices[MAX_MAP_VERTS];
    int nvertices;
    uint8_t vis[(MAX_MAP_LEAFS * MAX_MAP_LEAFS) >> 3]; // Bit packed, not rle until written to file
    bspfnode_t nodes[MAX_MAP_NODES];
    int nnodes;
    bspftexinfo_t texinfo[MAX_MAP_TEXINFO];
    int ntexinfo;
    bspfface_t faces[MAX_MAP_FACES];
    int nfaces;
    // TODO: Lighting
    bspfclipnode_t clipnodes[MAX_MAP_CLIPNODES];
    int nclipnodes;
    bspfleaf_t leaves[MAX_MAP_LEAFS];
    int nleaves;
    uint16_t marksurfs[MAX_MAP_MARKSURFACES];
    int nmarksurfs;
    bspfedge edges[MAX_MAP_EDGES];
    int nedges;
    int32_t surfedes[MAX_MAP_SURFEDGES];
    int nsurfedges;
    bspmodel_t models[MAX_MAP_MODELS];
    int nmodels;
} bspffile_t;

void WriteBspFile(bspffile_t* bsp, char* file);

#endif /* bspfile_h */
