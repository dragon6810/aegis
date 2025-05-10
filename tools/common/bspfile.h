#ifndef _BSPFILE_H
#define _BSPFILE_H

#include <stdint.h>

#include <vector.h>

#define BSP_FILE_VERSION    31

#define	MAX_MAP_HULLS		4

#define MAX_MAP_RANGE       8192

#define	MAX_MAP_MODELS		256
#define	MAX_MAP_BRUSHES		4096
#define	MAX_MAP_ENTITIES	1024
#define	MAX_MAP_ENTSTRING	65536

#define	MAX_MAP_PLANES		 8192
#define	MAX_MAP_NODES		 32767
#define	MAX_MAP_CLIPNODES	 32767
#define	MAX_MAP_LEAFS		 32767
#define MAX_MAP_CLIPLEAFS    32767
#define	MAX_MAP_VERTS		 65535
#define	MAX_MAP_FACES		 65535
#define	MAX_MAP_MARKSURFACES 65535
#define	MAX_MAP_MARKPORTALS  65535
#define	MAX_MAP_TEXINFO		 4096
#define	MAX_MAP_EDGES		 256000
#define	MAX_MAP_SURFEDGES	 512000
#define MAX_MAP_PORTEDGES    1024000
#define MAX_MAP_PORTALS      256000
#define	MAX_MAP_MIPTEX		 0x200000
#define	MAX_MAP_LIGHTING	 0x100000
#define	MAX_MAP_VISIBILITY	 0x100000
#define MAX_MAP_HULLSURFS    65535

#define ENTITY_MAX_KEY 32
#define ENTITY_MAX_VAL 1024

#define LEAF_CONTENT_EMPTY 0
#define LEAF_CONTENT_SOLID 1
#define LEAF_CONTENT_WATER 2

typedef struct
{
    int32_t version;
    uint32_t ntables;
    uint32_t tablelocation;
} bspfile_header_t;

typedef struct
{
    char name[8]; // must be null-terminated
    uint32_t version;
    uint32_t size;
    uint32_t location;
} bspfile_tableentry_t;

typedef struct
{
    vec3_t n;
    float d;
    uint32_t nfaces;
    uint32_t firstface;
} bspfile_plane_t;

typedef struct
{
    uint32_t plane;
    int32_t children[2]; // if negative, bitwise inverse into leaf indexes
    vec3_t bbox[2];
} bspfile_node_t;

typedef struct
{
    uint32_t plane;
    int32_t children[2]; // if negative, bitwise inverse into clip leaf indexes
} bspfile_clipnode_t;

typedef struct
{
    vec3_t bbox[2];
    uint32_t nfaces;
    uint32_t firstface;
} bspfile_leaf_t;

typedef struct
{
    uint32_t contents;
    uint32_t nmarkportals;
    uint32_t firstmarkportal;
} bspfile_clipleaf_t;

#endif