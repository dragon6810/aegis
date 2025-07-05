#ifndef _BSP_H
#define _BSP_H

#include <std/list/list.h>
#include <std/list/list_types.h>

#include <bspfile.h>
#include <vector.h>

typedef struct bsp_leaf_s
{
    int contents;
    int hull;
    list_int_t portals;
    int nfaces;
    int faces[0]; // TODO: turn this into a list_int_t
} bsp_leaf_t;

typedef struct bsp_plane_s
{
    vec3_t n;
    float d;
    int children[2]; // <=0 indices bitwise invert to get leaf index
    list_int_t faces;
    int hull;
    vec3_t bounds[2];
    list_int_t portals;
} bsp_plane_t;

typedef struct bsp_portal_s
{
    int hull;
    bsp_plane_t *pl;
    bsp_leaf_t *leafs[2];
    poly_t *poly;
    
    int curside; // used during construction, useless otherwise
} bsp_portal_t;

typedef struct bsp_texinfo_s
{
    vec3_t basis[2];
    float shift[2];
    char texname[16];
} bsp_texinfo_t;

typedef struct bsp_face_s
{
    poly_t *poly;
    int texinfo;
    int plane;

    uint16_t fileface;
} bsp_face_t;

typedef struct bsp_model_s
{
    int headplane[MAX_MAP_HULLS];
    int firstface[MAX_MAP_HULLS], nfaces[MAX_MAP_HULLS];
    vec3_t bounds[MAX_MAP_HULLS][2];
} bsp_model_t;

typedef struct bsp_epair_s
{
    char key[ENTITY_MAX_KEY], val[ENTITY_MAX_VAL];
} bsp_epair_t;

typedef struct bsp_entity_s
{
    int npairs;
    bsp_epair_t pairs[0];
} bsp_entity_t;

extern vec3_t bsp_mapbounds[2];

extern char bsp_entstring[MAX_MAP_ENTSTRING];
extern bsp_leaf_t *bsp_leaves[MAX_MAP_HULLS][MAX_MAP_LEAFS];
extern bsp_plane_t bsp_planes[MAX_MAP_HULLS][MAX_MAP_PLANES];
extern bsp_portal_t bsp_portals[MAX_MAP_HULLS][MAX_MAP_PORTALS];
extern bsp_texinfo_t bsp_texinfos[MAX_MAP_TEXINFO];
extern bsp_face_t bsp_faces[MAX_MAP_HULLS][MAX_MAP_FACES];
extern bsp_model_t bsp_models[MAX_MAP_MODELS];
extern bsp_entity_t *bsp_entities[MAX_MAP_ENTITIES];

extern int bsp_nleaves[MAX_MAP_HULLS];
extern int bsp_nplanes[MAX_MAP_HULLS];
extern int bsp_nportals[MAX_MAP_HULLS];
extern int bsp_ntexinfos;
extern int bsp_nfaces[MAX_MAP_HULLS];
extern int bsp_nmodels;
extern int bsp_nentities;

int bsp_findtexinfo(const bsp_texinfo_t* texinfo);
void bsp_loadhulls(void);
void bsp_loadents(void);
void bsp_partition(void);
void bsp_portalize(void);
void bsp_loadintofile(void);

#endif