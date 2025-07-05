#include <bsp/bsp.h>

vec3_t bsp_mapbounds[2] = { {}, {}, };

char bsp_entstring[MAX_MAP_ENTSTRING] = {};
bsp_leaf_t *bsp_leaves[MAX_MAP_HULLS][MAX_MAP_LEAFS] = {};
bsp_plane_t bsp_planes[MAX_MAP_HULLS][MAX_MAP_PLANES] = {};
bsp_portal_t bsp_portals[MAX_MAP_HULLS][MAX_MAP_PORTALS] = {};
bsp_texinfo_t bsp_texinfos[MAX_MAP_TEXINFO] = {};
bsp_face_t bsp_faces[MAX_MAP_HULLS][MAX_MAP_FACES] = {};
bsp_model_t bsp_models[MAX_MAP_MODELS] = {};
bsp_entity_t *bsp_entities[MAX_MAP_ENTITIES] = {};

int bsp_nleaves[MAX_MAP_HULLS] = {};
int bsp_nplanes[MAX_MAP_HULLS] = {};
int bsp_nportals[MAX_MAP_HULLS] = {};
int bsp_ntexinfos = 0;
int bsp_nfaces[MAX_MAP_HULLS] = {};
int bsp_nmodels = 0;
int bsp_nentities = 0;