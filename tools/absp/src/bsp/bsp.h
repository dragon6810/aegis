#ifndef _BSP_H
#define _BSP_H

#include <bspfile.h>
#include <vector.h>

typedef struct bsp_leaf_s
{
    int nfaces;
    struct bsp_face_s *faces[0];
    int contents;
} bsp_leaf_t;

typedef struct bsp_plane_s
{
    vec3_t n;
    float d;
    struct bsp_plane_s *children[2];
    bsp_leaf_t *leaf;
} bsp_plane_t;

typedef struct bsp_texinfo_s
{
    vec3_t basis[2];
    float shift[2];
    char texname[16];
} bsp_texinfo_t;

typedef struct bsp_face_s
{
    poly_t *poly;
    struct bsp_texinfo_s *texinfo;
    bsp_plane_t *plane;
} bsp_face_t;

typedef struct bsp_model_s
{
    int headplane[MAX_MAP_HULLS];
    int firstface[MAX_MAP_HULLS], nfaces[MAX_MAP_HULLS];
} bsp_model_t;

extern vec3_t bsp_mapbounds[2];

extern bsp_leaf_t *bsp_leaves[MAX_MAP_HULLS][MAX_MAP_LEAFS];
extern bsp_plane_t bsp_planes[MAX_MAP_HULLS][MAX_MAP_PLANES];
extern bsp_texinfo_t bsp_texinfos[MAX_MAP_TEXINFO];
extern bsp_face_t bsp_faces[MAX_MAP_HULLS][MAX_MAP_FACES];
extern bsp_model_t bsp_models[MAX_MAP_MODELS];

extern int bsp_nleaves[MAX_MAP_HULLS];
extern int bsp_nplanes[MAX_MAP_HULLS];
extern int bsp_ntexinfos[MAX_MAP_HULLS];
extern int bsp_nfaces[MAX_MAP_HULLS];
extern int bsp_nmodels;

void bsp_loadhulls(void);
void bsp_loadents(void);

#endif