//
//  bspfile.c
//  Partition
//
//  Created by Henry Dunn on 10/8/24.
//

#include "bspfile.h"

#include <stdio.h>
#include <string.h>

void WriteBspFile(bspffile_t* bsp, char* file)
{
    const int version = 31;
    const int headersize = 124;
        
    int i;
    
    FILE* ptr;
    
    int entoffs, entlen;
    int planeoffs, planelen;
    int texoffs, texlen;
    int vertoffs, vertlen;
    int visoffs, vislen;
    int nodeoffs, nodelen;
    int texinfooffs, texinfolen;
    int faceoffs, facelen;
    int lightoffs, lightlen;
    int clipoffs, cliplen;
    int leafoffs, leaflen;
    int marksurfoffs, marksurflen;
    int edgeoffs, edgelen;
    int surfedgeoffs, surfedgelen;
    int modeloffs, modellen;
    int curoffs;
    
    ptr = fopen(file, "wb");
    if(!ptr)
        return;
    
    entlen = strlen(bsp->entitydata);
    planelen = bsp->nplanes * sizeof(bspfplane_t);
    texlen = bsp->ntextures * sizeof(bspfmiptex_t) + sizeof(int) * (bsp->ntextures + 1);
    vertlen = bsp->nvertices * sizeof(vec3_t);
    vislen = 0;
    nodelen = bsp->nnodes * sizeof(bspfnode_t);
    texinfolen = bsp->ntexinfo * sizeof(bspftexinfo_t);
    facelen = bsp->nfaces * sizeof(bspfface_t);
    lightlen = 0;
    cliplen = bsp->nclipnodes * sizeof(bspfclipnode_t);
    leaflen = bsp->nleaves * sizeof(bspfleaf_t);
    marksurflen = bsp->nmarksurfs * sizeof(uint16_t);
    edgelen = bsp->nedges * sizeof(bspfedge_t);
    surfedgelen = bsp->nsurfedges * sizeof(int);
    modellen = bsp->nmodels * sizeof(bspmodel_t);
    
    
    entoffs = headersize;
    planeoffs = entoffs + entlen;
    texoffs = planeoffs + planelen;
    vertoffs = texoffs + texlen;
    visoffs = vertoffs + vertlen;
    nodeoffs = visoffs + vislen;
    texinfooffs = nodeoffs + nodelen;
    faceoffs = texinfooffs + texinfolen;
    lightoffs = faceoffs + facelen;
    clipoffs = lightoffs + lightlen;
    leafoffs = clipoffs + cliplen;
    marksurfoffs = leafoffs + leaflen;
    edgeoffs = marksurfoffs + marksurflen;
    surfedgeoffs = edgeoffs + edgelen;
    modeloffs = surfedgeoffs + surfedgelen;
    
    fwrite(&version, sizeof(version), 1, ptr);
    
    fwrite(&entoffs, sizeof(int), 1, ptr);
    fwrite(&entlen, sizeof(int), 1, ptr);
    fwrite(&planeoffs, sizeof(int), 1, ptr);
    fwrite(&planelen, sizeof(int), 1, ptr);
    fwrite(&texoffs, sizeof(int), 1, ptr);
    fwrite(&texlen, sizeof(int), 1, ptr);
    fwrite(&vertoffs, sizeof(int), 1, ptr);
    fwrite(&vertlen, sizeof(int), 1, ptr);
    fwrite(&visoffs, sizeof(int), 1, ptr);
    fwrite(&vislen, sizeof(int), 1, ptr);
    fwrite(&nodeoffs, sizeof(int), 1, ptr);
    fwrite(&nodelen, sizeof(int), 1, ptr);
    fwrite(&texinfooffs, sizeof(int), 1, ptr);
    fwrite(&texinfolen, sizeof(int), 1, ptr);
    fwrite(&faceoffs, sizeof(int), 1, ptr);
    fwrite(&facelen, sizeof(int), 1, ptr);
    fwrite(&lightoffs, sizeof(int), 1, ptr);
    fwrite(&lightlen, sizeof(int), 1, ptr);
    fwrite(&clipoffs, sizeof(int), 1, ptr);
    fwrite(&cliplen, sizeof(int), 1, ptr);
    fwrite(&leafoffs, sizeof(int), 1, ptr);
    fwrite(&leaflen, sizeof(int), 1, ptr);
    fwrite(&marksurfoffs, sizeof(int), 1, ptr);
    fwrite(&marksurflen, sizeof(int), 1, ptr);
    fwrite(&edgeoffs, sizeof(int), 1, ptr);
    fwrite(&edgelen, sizeof(int), 1, ptr);
    fwrite(&surfedgeoffs, sizeof(int), 1, ptr);
    fwrite(&surfedgelen, sizeof(int), 1, ptr);
    fwrite(&modeloffs, sizeof(int), 1, ptr);
    fwrite(&modellen, sizeof(int), 1, ptr);
    
    fwrite(bsp->entitydata, 1, strlen(bsp->entitydata), ptr);
    fwrite(bsp->planes, sizeof(bspfplane_t), bsp->nplanes, ptr);
    fwrite(&bsp->ntextures, sizeof(int), 1, ptr);
    for(i=0; i<bsp->ntextures; i++)
    {
        curoffs = sizeof(int) * (bsp->ntextures + 1) + i * sizeof(bspfmiptex_t);
        fwrite(&curoffs, sizeof(int), 1, ptr);
    }
    fwrite(bsp->textures, sizeof(bspfmiptex_t), bsp->ntextures, ptr);
    fwrite(bsp->vertices, sizeof(vec3_t), bsp->nvertices, ptr);
    // Leave Vis Blank
    fwrite(bsp->nodes, sizeof(bspfnode_t), bsp->nnodes, ptr);
    fwrite(bsp->texinfo, sizeof(bspftexinfo_t), bsp->ntexinfo, ptr);
    fwrite(bsp->faces, sizeof(bspfface_t), bsp->nfaces, ptr);
    // Leave Light Blank
    fwrite(bsp->clipnodes, sizeof(bspfclipnode_t), bsp->nclipnodes, ptr);
    fwrite(bsp->leaves, sizeof(bspfleaf_t), bsp->nleaves, ptr);
    fwrite(bsp->marksurfs, sizeof(uint16_t), bsp->nmarksurfs, ptr);
    long test = ftell(ptr);
    fwrite(bsp->edges, sizeof(bspfedge_t), bsp->nedges, ptr);
    fwrite(bsp->surfedes, sizeof(int), bsp->nsurfedges, ptr);
    fwrite(bsp->models, sizeof(bspmodel_t), bsp->nmodels, ptr);
    
    fclose(ptr);
}
