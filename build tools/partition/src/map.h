//
//  map.h
//  Partition
//
//  Created by Henry Dunn on 9/18/24.
//

#ifndef map_h
#define map_h

#include "polygon.h"
#include "bspfile.h"
#include "mapdefs.h"
#include <math.h>
#include <string.h>

extern char *filename;
extern FILE* gfiles[NHULLS];
extern brushsetnode_t *brushsets[NHULLS];
extern splitplane_t rootnode[MAX_MAP_MODELS][NHULLS];
extern int nmodels;
extern int nleaves;
extern bspffile_t bspfile;
extern vec3_t* positions;
extern int npositions;

void LoadBrushSets(char* file);
void LoadEnts(char* file);
void ProcessWorld();
splitplane_t MakeSplitNode(surfnode_t *surfs);
void CutWorld_r(splitplane_t* parent);
surfnode_t* FindIdealSplitSurf(surfnode_t *surfs);
void WriteFile();

// -1 for back, 0 for on, 1 for front, 2 for both
int GetSurfSide(surf_t* surf, vec3_t n, float d);
surf_t* CopySurf(surf_t* surf);

void LoadNodes_r(splitplane_t *node, boolean rendertree);
int LoadSurfs(surfnode_t* surf);
void LoadFace(surf_t* face);
int LoadLeaf(leaf_t* leaf);
int FindEdge(int v1, int v2);
int FindVertex(vec3_t v);
int FindTexinfo(bspftexinfo_t* texinfo);
int FindMiptex(char* name);
int FindPlane(vec3_t n, float d);
int FindFace(bspfface_t* face);
void FindFacePlanes();
void LoadModels();

#endif /* map_h */
