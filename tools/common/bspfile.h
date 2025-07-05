#ifndef _BSPFILE_H
#define _BSPFILE_H

#include <stdint.h>

#include <vector.h>

/*
BSP FILE FORMAT

header:
    char magic[4];      // 'B' 'S' 'P' '\0'
    uint32_t version;   // 31
    uint32_t nlumps;    // total number of lumps
    uint64_t tableoffs; // location of lump table

lump table entry:
    char tag[8];   // null-terminated
    uint64_t size; // size of all data of lump
    uint64_t offs; // location of lump
*/

#define BSP_FILE_VERSION    31

#define	MAX_MAP_HULLS		4

#define MAX_MAP_RANGE       8192

#define	MAX_MAP_MODELS		256
#define	MAX_MAP_BRUSHES		4096
#define	MAX_MAP_ENTITIES	1024
#define	MAX_MAP_ENTSTRING	65536

#define	MAX_MAP_PLANES		  8192
#define	MAX_MAP_NODES		  32767
#define	MAX_MAP_LEAFS		  32767
#define	MAX_MAP_CLIPNODES	  32767
#define MAX_MAP_CLIPLEAFS     32767
#define	MAX_MAP_FACES		  65535
#define	MAX_MAP_EDGES		  256000
#define	MAX_MAP_VERTS		  65535
#define	MAX_MAP_MARKSURFACES  65535
#define	MAX_MAP_MARKEDGES	  512000
#define	MAX_MAP_TEXINFO		  4096
#define MAX_MAP_PORTALS       65535
#define	MAX_MAP_MARKPORTALS   65535
#define	MAX_MAP_LIGHTING	  0x100000
#define	MAX_MAP_VISIBILITY	  0x100000

#define ENTITY_MAX_KEY 32
#define ENTITY_MAX_VAL 1024

#define FACE_MAX_LIGHTSYLE 4

#define LEAF_CONTENT_EMPTY 0
#define LEAF_CONTENT_SOLID 1
#define LEAF_CONTENT_WATER 2

static const char bspfile_tag_models[8]      = "MODEL";
static const char bspfile_tag_entities[8]    = "ENTITY";
static const char bspfile_tag_planes[8]      = "PLANE";
static const char bspfile_tag_nodes[8]       = "BSP";
static const char bspfile_tag_leafs[8]       = "LEAF";
static const char bspfile_tag_clipnodes[8]   = "HULLBSP";
static const char bspfile_tag_clipleafs[8]   = "HULLEAF";
static const char bspfile_tag_faces[8]       = "FACE";
static const char bspfile_tag_edges[8]       = "EDGE";
static const char bspfile_tag_verts[8]       = "VERTEX";
static const char bspfile_tag_marksurfs[8]   = "MRKFACE";
static const char bspfile_tag_markedges[8]   = "MRKEDGE";
static const char bspfile_tag_texinfo[8]     = "TEXINFO";
static const char bspfile_tag_portals[8]     = "PORTAL";
static const char bspfile_tag_markportals[8] = "IPORTAL";
static const char bspfile_tag_lighting[8]    = "LIGHT";
static const char bspfile_tag_vis[8]         = "PVS";

#pragma pack(push,1)
typedef struct
{
    char name[8]; // must be null-terminated
    uint64_t size;
    uint64_t location;
} bspfile_tableentry_t;

typedef struct
{
    uint32_t index;
    uint32_t renderhead;
    uint32_t headnodes[MAX_MAP_HULLS];
    uint16_t nportals[MAX_MAP_HULLS];
    uint16_t firstportal[MAX_MAP_HULLS];
} bspfile_model_t;

typedef struct
{
    vec3_t n;
    float d;
} bspfile_plane_t;

typedef struct
{
    uint32_t plane;
    int32_t children[2]; // if negative, bitwise inverse into leaf indexes
    vec3_t bbox[2];
    uint32_t nfaces;
    uint32_t firstface;
} bspfile_node_t;

typedef struct
{
    vec3_t bbox[2];
    uint16_t nfaces;
    uint32_t firstmarkface;
    uint16_t clipleaf;
} bspfile_leaf_t;

typedef struct
{
    uint16_t plane;
    uint16_t children[2]; // if negative, bitwise inverse into clip leaf indexes
} bspfile_clipnode_t;

typedef struct
{
    int8_t contents;
    uint16_t nmarkportals;
    uint32_t firstmarkportal;
} bspfile_clipleaf_t;

typedef struct
{
    uint32_t portal;
    uint32_t firstmarkedge; // annoyingly have to do this in addition to portal because multiple faces can be on 1 portal
    uint16_t nmarkedges;
    uint16_t texinfo;
    int32_t lights[FACE_MAX_LIGHTSYLE]; // terminated with -1 or max of FACE_MAX_LIGHTSTYLE
} bspfile_face_t;

typedef struct
{
    int16_t vertex[2];
} bspfile_edge_t;

typedef vec3_t bspfile_vertex_t;

typedef uint16_t bspfile_marksurf_t;

typedef int32_t bspfile_markedge_t; // if negative, negate to get edge and flip order

typedef struct
{
    vec3_t basis[2];
    float shift[2];
    char miptex[16];
} bspfile_texinfo_t;

typedef struct
{
    uint32_t firstmarkedge;
    uint16_t nmarkedges;
    int16_t plane;          // if negative, bitwise inverse and normal is reversed
    int32_t leaves[2];      // indices to clipleaves
} bspfile_portal_t;
#pragma pack(pop)

typedef uint16_t bspfile_markportal_t;

extern int bspfile_nmodels;
extern bspfile_model_t bspfile_models[MAX_MAP_MODELS];
extern char bspfile_entities[MAX_MAP_ENTSTRING];
extern int bspfile_nplanes;
extern bspfile_plane_t bspfile_planes[MAX_MAP_PLANES];
extern int bspfile_nnodes;
extern bspfile_node_t bspfile_nodes[MAX_MAP_NODES];
extern int bspfile_nleafs;
extern bspfile_leaf_t bspfile_leafs[MAX_MAP_LEAFS];
extern int bspfile_nclipnodes;
extern bspfile_clipnode_t bspfile_clipnodes[MAX_MAP_CLIPNODES];
extern int bspfile_nclipleafs;
extern bspfile_clipleaf_t bspfile_clipleafs[MAX_MAP_CLIPLEAFS];
extern int bspfile_nfaces;
extern bspfile_face_t bspfile_faces[MAX_MAP_FACES];
extern int bspfile_nedges;
extern bspfile_edge_t bspfile_edges[MAX_MAP_EDGES];
extern int bspfile_nverts;
extern bspfile_vertex_t bspfile_verts[MAX_MAP_VERTS];
extern int bspfile_nmarksurfs;
extern bspfile_marksurf_t bspfile_marksurfs[MAX_MAP_MARKSURFACES];
extern int bspfile_nmarkedges;
extern bspfile_markedge_t bspfile_markedges[MAX_MAP_MARKEDGES];
extern int bspfile_ntexinfo;
extern bspfile_texinfo_t bspfile_texinfo[MAX_MAP_TEXINFO];
extern int bspfile_nportals;
extern bspfile_portal_t bspfile_portals[MAX_MAP_PORTALS];
extern int bspfile_nmarkportals;
extern bspfile_markportal_t bspfile_markportals[MAX_MAP_MARKPORTALS];
extern int bspfile_nlighting;
extern uint8_t bspfile_lighting[MAX_MAP_LIGHTING];
extern int bspfile_nvis;
extern uint8_t bspfile_vis[MAX_MAP_VISIBILITY];

void bspfile_writebspfile(const char* path);

#endif