#include <bspfile.h>

#include <stdlib.h>
#include <string.h>

int bspfile_nmodels = 0;
bspfile_model_t bspfile_models[MAX_MAP_MODELS] = {};
char bspfile_entities[MAX_MAP_ENTSTRING] = {};
int bspfile_nplanes = 0;
bspfile_plane_t bspfile_planes[MAX_MAP_PLANES] = {};
int bspfile_nnodes = 0;
bspfile_node_t bspfile_nodes[MAX_MAP_NODES] = {};
int bspfile_nleafs = 0;
bspfile_leaf_t bspfile_leafs[MAX_MAP_LEAFS] = {};
int bspfile_nclipnodes = 0;
bspfile_clipnode_t bspfile_clipnodes[MAX_MAP_CLIPNODES] = {};
int bspfile_nclipleafs = 0;
bspfile_clipleaf_t bspfile_clipleafs[MAX_MAP_CLIPLEAFS] = {};
int bspfile_nfaces = 0;
bspfile_face_t bspfile_faces[MAX_MAP_FACES] = {};
int bspfile_nedges = 0;
bspfile_edge_t bspfile_edges[MAX_MAP_EDGES] = {};
int bspfile_nverts = 0;
bspfile_vertex_t bspfile_verts[MAX_MAP_VERTS] = {};
int bspfile_nmarksurfs = 0;
bspfile_marksurf_t bspfile_marksurfs[MAX_MAP_MARKSURFACES] = {};
int bspfile_nmarkedges = 0;
bspfile_markedge_t bspfile_markedges[MAX_MAP_MARKEDGES] = {};
int bspfile_ntexinfo = 0;
bspfile_texinfo_t bspfile_texinfo[MAX_MAP_TEXINFO] = {};
int bspfile_nportals = 0;
bspfile_portal_t bspfile_portals[MAX_MAP_PORTALS] = {};
int bspfile_nmarkportals = 0;
bspfile_markportal_t bspfile_markportals[MAX_MAP_MARKPORTALS] = {};
int bspfile_nlighting = 0;
uint8_t bspfile_lighting[MAX_MAP_LIGHTING] = {};
int bspfile_nvis = 0;
uint8_t bspfile_vis[MAX_MAP_VISIBILITY] = {};

void bspfile_writebspfile(const char* path)
{
    const char magic[4] = "BSP";
    const uint32_t version = 31;
    const uint32_t nlumps = 17;

    FILE *ptr;
    uint64_t tableoffsoffs, tableoffs;
    uint64_t modelsoffs;
    uint64_t entsoffs;
    uint64_t planesoffs;
    uint64_t nodesoffs;
    uint64_t leafsoffs;
    uint64_t clipnodesoffs;
    uint64_t clipleafsoffs;
    uint64_t facesoffs;
    uint64_t edgesoffs;
    uint64_t vertsoffs;
    uint64_t marksurfsoffs;
    uint64_t markedgesoffs;
    uint64_t texinfooffs;
    uint64_t portalsoffs;
    uint64_t markportalsoffs;
    uint64_t lightingoffs;
    uint64_t visoffs;
    bspfile_tableentry_t table[nlumps];

    ptr = fopen(path, "wb");
    if(!ptr)
    {
        fprintf(stderr, "error: can't open bsp file for writing \"%s\"\n", path);
        abort();
    }

    fwrite(magic, 1, sizeof(magic), ptr);
    fwrite(&version, sizeof(version), 1, ptr);
    fwrite(&nlumps, sizeof(nlumps), 1, ptr);
    tableoffs = 0;
    tableoffsoffs = ftell(ptr);
    fwrite(&tableoffs, sizeof(tableoffs), 1, ptr);

    modelsoffs = ftell(ptr);
    fwrite(&bspfile_models, sizeof(bspfile_model_t), bspfile_nmodels, ptr);
    entsoffs = ftell(ptr);
    fprintf(ptr, "%s", bspfile_entities);
    planesoffs = ftell(ptr);
    fwrite(&bspfile_planes, sizeof(bspfile_plane_t), bspfile_nplanes, ptr);
    nodesoffs = ftell(ptr);
    fwrite(&bspfile_nodes, sizeof(bspfile_node_t), bspfile_nnodes, ptr);
    leafsoffs = ftell(ptr);
    fwrite(&bspfile_leafs, sizeof(bspfile_leaf_t), bspfile_nleafs, ptr);
    clipnodesoffs = ftell(ptr);
    fwrite(&bspfile_clipnodes, sizeof(bspfile_clipnode_t), bspfile_nclipnodes, ptr);
    clipleafsoffs = ftell(ptr);
    fwrite(&bspfile_clipleafs, sizeof(bspfile_clipleaf_t), bspfile_nclipleafs, ptr);
    facesoffs = ftell(ptr);
    fwrite(&bspfile_faces, sizeof(bspfile_face_t), bspfile_nfaces, ptr);
    edgesoffs = ftell(ptr);
    fwrite(&bspfile_edges, sizeof(bspfile_edge_t), bspfile_nedges, ptr);
    vertsoffs = ftell(ptr);
    fwrite(&bspfile_verts, sizeof(bspfile_vertex_t), bspfile_nverts, ptr);
    marksurfsoffs = ftell(ptr);
    fwrite(&bspfile_marksurfs, sizeof(bspfile_marksurf_t), bspfile_nmarksurfs, ptr);
    markedgesoffs = ftell(ptr);
    fwrite(&bspfile_markedges, sizeof(bspfile_markedge_t), bspfile_nmarkedges, ptr);
    texinfooffs = ftell(ptr);
    fwrite(&bspfile_texinfo, sizeof(bspfile_texinfo_t), bspfile_ntexinfo, ptr);
    portalsoffs = ftell(ptr);
    fwrite(&bspfile_portals, sizeof(bspfile_portal_t), bspfile_nportals, ptr);
    markportalsoffs = ftell(ptr);
    fwrite(&bspfile_markportals, sizeof(bspfile_markportal_t), bspfile_nmarkportals, ptr);
    lightingoffs = ftell(ptr);
    fwrite(&bspfile_lighting, sizeof(uint8_t), bspfile_nlighting, ptr);
    visoffs = ftell(ptr);
    fwrite(&bspfile_vis, sizeof(uint8_t), bspfile_nvis, ptr);

    tableoffs = ftell(ptr);
    fseek(ptr, tableoffsoffs, SEEK_SET);
    fwrite(&tableoffs, sizeof(tableoffs), 1, ptr);
    fseek(ptr, tableoffs, SEEK_SET);

    strcpy(table[ 0].name, bspfile_tag_models);
    table[ 0].size = bspfile_nmodels * sizeof(bspfile_model_t);
    table[ 0].location = modelsoffs;
    strcpy(table[ 1].name, bspfile_tag_entities);
    table[ 1].size = strlen(bspfile_entities) + 1;
    table[ 1].location = entsoffs;
    strcpy(table[ 2].name, bspfile_tag_planes);
    table[ 2].size = bspfile_nplanes * sizeof(bspfile_plane_t);
    table[ 2].location = planesoffs;
    strcpy(table[ 3].name, bspfile_tag_nodes);
    table[ 3].size = bspfile_nnodes * sizeof(bspfile_node_t);
    table[ 3].location = nodesoffs;
    strcpy(table[ 4].name, bspfile_tag_leafs);
    table[ 4].size = bspfile_nleafs * sizeof(bspfile_leaf_t);
    table[ 4].location = leafsoffs;
    strcpy(table[ 5].name, bspfile_tag_clipnodes);
    table[ 5].size = bspfile_nclipnodes * sizeof(bspfile_clipnode_t);
    table[ 5].location = clipnodesoffs;
    strcpy(table[ 6].name, bspfile_tag_clipleafs);
    table[ 6].size = bspfile_nclipleafs * sizeof(bspfile_clipleaf_t);
    table[ 6].location = clipleafsoffs;
    strcpy(table[ 7].name, bspfile_tag_faces);
    table[ 7].size = bspfile_nfaces * sizeof(bspfile_face_t);
    table[ 7].location = facesoffs;
    strcpy(table[ 8].name, bspfile_tag_edges);
    table[ 8].size = bspfile_nedges * sizeof(bspfile_edge_t);
    table[ 8].location = edgesoffs;
    strcpy(table[ 9].name, bspfile_tag_verts);
    table[ 9].size = bspfile_nverts * sizeof(bspfile_vertex_t);
    table[ 9].location = vertsoffs;
    strcpy(table[10].name, bspfile_tag_marksurfs);
    table[10].size = bspfile_nmarksurfs * sizeof(bspfile_marksurf_t);
    table[10].location = marksurfsoffs;
    strcpy(table[11].name, bspfile_tag_markedges);
    table[11].size = bspfile_nmarkedges * sizeof(bspfile_markedge_t);
    table[11].location = markedgesoffs;
    strcpy(table[12].name, bspfile_tag_texinfo);
    table[12].size = bspfile_ntexinfo * sizeof(bspfile_texinfo_t);
    table[12].location = texinfooffs;
    strcpy(table[13].name, bspfile_tag_portals);
    table[13].size = bspfile_nportals * sizeof(bspfile_portal_t);
    table[13].location = portalsoffs;
    strcpy(table[14].name, bspfile_tag_markportals);
    table[14].size = bspfile_nmarkportals * sizeof(bspfile_markportal_t);
    table[14].location = markportalsoffs;
    strcpy(table[15].name, bspfile_tag_lighting);
    table[15].size = bspfile_nlighting * sizeof(uint8_t);
    table[15].location = lightingoffs;
    strcpy(table[16].name, bspfile_tag_vis);
    table[16].size = bspfile_nvis * sizeof(uint8_t);
    table[16].location = visoffs;

    fwrite(table, sizeof(bspfile_tableentry_t), nlumps, ptr);

    fclose(ptr);
}