//
//  bsp.c
//  Partition
//
//  Created by Henry Dunn on 9/18/24.
//

#include "map.h"
#include "portal.h"

int curhull;

char *filename;
FILE* gfiles[NHULLS];
brushsetnode_t *brushsets[NHULLS];
splitplane_t rootnode[MAX_MAP_MODELS][NHULLS];
int nmodels = 0;
int nleaves = 0;
bspffile_t bspfile;

void LoadBrushSets(char* file)
{
    int i, j;
    
    int modelnum;
    surf_t *newsurf;
    brushset_t *newset;
    brushsetnode_t *newsetnode, *lastsetnode;
    surfnode_t *newsurfnode, *lastsurfnode;
    vnode_t *newv, *lastv, *firstv;
    vec3_t val, s, t;
    int sshift, tshift;
    char texname[17];
    
    char *newfile;
    
    filename = file;
    
    newfile = malloc(strlen(file) + 4);
    strcpy(newfile, file);
    strcat(newfile, ".g");
    for(i=0; i<NHULLS; i++)
    {
        newfile[strlen(file) + 2] = '0' + i;
        newfile[strlen(file) + 3] = 0;
        gfiles[i] = fopen(newfile, "r");
        if(!gfiles[i])
        {
            printf("File does not exist %s!\n", newfile);
            free(newfile);
            exit(1);
        }
        
        lastsetnode = 0;
        while(!feof(gfiles[i]))
        {
            if(fscanf(gfiles[i], " *%d\n", &modelnum))
            {
                if(modelnum > nmodels)
                    nmodels++;

                newset = (brushset_t*) calloc(1, sizeof(brushset_t));
                newsetnode = (brushsetnode_t*) calloc(1, sizeof(brushsetnode_t));
                newsetnode->brushet = newset;
                if(lastsetnode)
                {
                    lastsetnode->next = newsetnode;
                    newsetnode->last = lastsetnode;
                    lastsetnode = newsetnode;
                }
                else
                {
                    lastsetnode = brushsets[i] = newsetnode;
                }
                j = 1;
                lastsurfnode = 0;
                int k = 0;
                while(j != 0)
                {
                    lastv = firstv = 0;

                    newsurf = calloc(1, sizeof(surf_t));
                    lastv = 0;
                    j = 0;
                    while(fscanf(gfiles[i], "( %f %f %f )", &val[0], &val[1], &val[2]) == 3)
                    {
                        newv = AllocVert();
                        VectorCopy(newv->val, val);
                        if(lastv)
                        {
                            newv->last = lastv;
                            newv->next = firstv;
                            firstv->last = newv;
                            lastv->next = newv;
                            lastv = newv;
                        }
                        else
                        {
                            firstv = newv;
                            newv->next = newv->last = lastv = newv;
                        }
                        
                        j++;
                    }
                    if(!j)
                        continue;
                    
                    newsurf->geo.first = firstv;
                    
                    if(fscanf(gfiles[i], "[ %f %f %f %d ] ", &newsurf->s[0], &newsurf->s[1], &newsurf->s[2], &newsurf->sshift) < 4)
                    {
                        char test[17];
                        fread(test, 17, 1, gfiles[i]);
                        printf("uh oh\n");
                    }
                    fscanf(gfiles[i], "[ %f %f %f %d ] ", &newsurf->t[0], &newsurf->t[1], &newsurf->t[2], &newsurf->tshift);
                    fscanf(gfiles[i], "%s\n", newsurf->texname);
                    
                    newsurfnode = (surfnode_t*) calloc(1, sizeof(surfnode_t));
                    if(lastsurfnode)
                    {
                        lastsurfnode->next = newsurfnode;
                        newsurfnode->last = lastsurfnode;
                        lastsurfnode = newsurfnode;
                    }
                    else
                    {
                        lastsurfnode = newset->firstsurf = newsurfnode;
                    }
                    newsurfnode->surf = newsurf;
                    k++;
                }
            }
            else
            {
                free(newfile);
                return;
            }
        }
    }
    
    free(newfile);
}

void ProcessWorld()
{
    int i, j, k;
    brushsetnode_t *set;
    
    for(curhull=0; curhull<NHULLS; curhull++)
    {
        for(j=0, set=brushsets[curhull]; set; j++, set=set->next)
        {
            rootnode[j][curhull] = MakeSplitNode(set->brushet->firstsurf);
            CutWorld_r(&rootnode[j][curhull]);

            if (curhull)
                continue;

            Portalize(&rootnode[j][curhull]);

            for(k=0; k<npositions; k++)
                FillWorld(&rootnode[j][curhull], positions[k]);

            FreeTree(&rootnode[j][curhull]);

            rootnode[j][curhull] = MakeSplitNode(set->brushet->firstsurf);
            CutWorld_r(&rootnode[j][curhull]);
        }
    }
}

void WriteFile()
{
    char* outfile;
    LoadModels();
    FindFacePlanes();
    outfile = malloc(strlen(filename) + 4 + 1);
    if(!outfile)
        return;
    strcpy(outfile, filename);
    strcat(outfile, ".bsp");
    WriteBspFile(&bspfile, outfile);
    free(outfile);
}

void LoadNodes_r(splitplane_t *node, boolean rendertree)
{
    int i;
    
    int nodeindex, clipindex;
    bspfplane_t bspplane;
    bspfnode_t bspnode;
    bspfclipnode_t bspclipnode;
    
    vec3_t mins[3], maxs[3];
    float min, max;
    
    VectorCopy(bspplane.n, node->n);
    bspplane.d = node->d;
    
    if(node->n[0] == 1)
        bspplane.type = PLANE_X;
    else if(node->n[1] == 1)
        bspplane.type = PLANE_Y;
    else if(node->n[2] == 1)
        bspplane.type = PLANE_Z;
    else if(node->n[0] >= node->n[1] && node->n[0] >= node->n[2])
        bspplane.type = PLANE_ANYX;
    else if(node->n[1] >= node->n[0] && node->n[1] >= node->n[2])
        bspplane.type = PLANE_ANYY;
    else
        bspplane.type = PLANE_ANYZ;
    
    bspfile.planes[bspfile.nplanes] = bspplane;
    bspclipnode.plane = bspfile.nplanes++;
    if(rendertree)
    {
        bspnode.plane = bspfile.nplanes;
        bspnode.firstface = bspfile.nfaces;
        bspnode.nfaces = LoadSurfs(node->surfs);
        
        SurfListBB(node->childsurfs[0], &mins[0], &maxs[0]);
        SurfListBB(node->surfs,         &mins[1], &maxs[1]);
        SurfListBB(node->childsurfs[1], &mins[2], &maxs[2]);
        
        for(i=0; i<3; i++)
        {
            min = mins[0][i] < mins[1][i] && mins[0][i] < mins[2][i] ? mins[0][i] : mins[1][i] < mins[2][i] ? mins[1][i] : mins[2][i]; // what the fuck
            max = maxs[0][i] > maxs[1][i] && maxs[0][i] > maxs[2][i] ? maxs[0][i] : maxs[1][i] > maxs[2][i] ? maxs[1][i] : maxs[2][i];
            
            bspnode.min[i] = floorf(min);
            bspnode.max[i] = ceilf(max);
        }
        
        nodeindex = bspfile.nnodes;
        bspfile.nodes[bspfile.nnodes++] = bspnode;
    }
    
    clipindex = bspfile.nclipnodes;
    bspfile.clipnodes[bspfile.nclipnodes++] = bspclipnode;
    
    for (i = 0; i < 2; i++)
    {
        if (!node->children[i]->leaf)
        {
            if (rendertree)
                bspfile.nodes[nodeindex].children[i] = bspfile.nnodes;

            bspfile.clipnodes[clipindex].children[!i] = bspfile.nclipnodes;
            LoadNodes_r(node->children[i], rendertree);
        }
        else
        {
            if (rendertree)
                bspfile.nodes[nodeindex].children[i] = ~LoadLeaf(node->children[i]->leaf);

            bspfile.clipnodes[clipindex].children[!i] = node->children[i]->leaf->contents;
        }
    }
}

int LoadLeaf(leaf_t* leaf)
{
    int i;
    
    surfnode_t *surfnode;
    bspfface_t face;
    vec3_t mins, maxs;
    
    bspfleaf_t fleaf;
    
    fleaf.contents = leaf->contents;
    SurfListBB(leaf->surfs, &mins, &maxs);
    for(i=0; i<3; i++)
    {
        fleaf.mins[i] = (int16_t) floorf(mins[i]);
        fleaf.maxs[i] = (int16_t) floorf(maxs[i]);
    }
    
    fleaf.firstmarksurf = bspfile.nmarksurfs;
    fleaf.nmarksurfs = 0;
    for(surfnode=leaf->surfs; surfnode; surfnode=surfnode->next)
    {
        LoadFace(surfnode->surf);
        face = bspfile.faces[bspfile.nfaces - 1];
        memset(&bspfile.faces[bspfile.nfaces - 1], 0, sizeof(bspfface_t));
        bspfile.nfaces--;
        
        bspfile.marksurfs[bspfile.nmarksurfs++] = FindFace(&face);
        fleaf.nmarksurfs++;
    }
    
    bspfile.leaves[bspfile.nleaves] = fleaf;
    return bspfile.nleaves++;
}

int LoadSurfs(surfnode_t* surf)
{
    int i;
    
    surfnode_t *curnode;
    
    for(i=0, curnode=surf; curnode; i++, curnode=curnode->next)
    {
        LoadFace(curnode->surf);
    }
    
    return i;
}

void LoadFace(surf_t* face)
{
    int i;
    
    bspfface_t newface;
    bspftexinfo_t texinfo;
    vnode_t *v, *next;
    int v1, v2;
    int e;
    
    memset(&newface, 0, sizeof(bspfface_t));
    newface.firstedge = bspfile.nsurfedges;
    newface.nedges = 0;
    for(v=face->geo.first, next=v->next;; v=next=v->next)
    {
        v1 = FindVertex(v->val);
        v2 = FindVertex(next->val);
        
        e = FindEdge(v1, v2);
        if(v1 == bspfile.edges[e][1])
            e = -e;
        
        newface.nedges++;
        bspfile.surfedes[bspfile.nsurfedges] = e;
        bspfile.nsurfedges++;
        
        if(v->next == face->geo.first)
            break;
    }
    
    VectorCopy(texinfo.s, face->s);
    VectorCopy(texinfo.t, face->t);
    texinfo.sshift = face->sshift;
    texinfo.tshift = face->tshift;
    texinfo.miptex = FindMiptex(face->texname);
    newface.texinfo = FindTexinfo(&texinfo);
    for(i=0; i<4; i++)
        newface.styles[i] = 255;
    newface.lightmapoffs = -1;
    
    bspfile.faces[bspfile.nfaces++] = newface;
}

int FindEdge(int v1, int v2)
{
    int i;
    
    for(i=0; i<bspfile.nedges; i++)
    {
        if((v1 == bspfile.edges[i][0] && v2 == bspfile.edges[i][1]) ||
           (v1 == bspfile.edges[i][1] && v2 == bspfile.edges[i][0]))
            return i;
    }
    
    bspfile.edges[i][0] = v1;
    bspfile.edges[i][1] = v2;
    return bspfile.nedges++;
}

int FindVertex(vec3_t v)
{
    int i;
    
    for(i=0; i<bspfile.nvertices; i++)
    {
        if(VectorComp(v, bspfile.vertices[i]))
            return i;
    }
    
    VectorCopy(bspfile.vertices[i], v);
    return bspfile.nvertices++;
}

int FindTexinfo(bspftexinfo_t* texinfo)
{
    int i;
    
    for(i=0; i<bspfile.ntexinfo; i++)
    {
        if(!memcmp(texinfo, &bspfile.texinfo[i], sizeof(bspftexinfo_t)))
            return i;
    }
    
    memcpy(&bspfile.texinfo[i], texinfo, sizeof(bspftexinfo_t));
    return bspfile.ntexinfo++;
}

int FindMiptex(char* name)
{
    int i;
    
    for(i=0; i<bspfile.ntextures; i++)
    {
        if(!memcmp(name, bspfile.textures[i].name, 16))
            return i;
    }
    
    memcpy(&bspfile.textures[i], name, 16);
    return bspfile.ntextures++;
}

int FindPlane(vec3_t n, float d)
{
    int i;
    
    for(i=0; i<bspfile.nplanes; i++)
    {
        if(VectorComp(bspfile.planes[i].n, n) && bspfile.planes[i].d == d)
            return i;
    }
    
    return -1;
}

int FindFace(bspfface_t* face)
{
    int i;
    
    for(i=0; i<bspfile.nfaces; i++)
    {
        if(!memcmp(&bspfile.faces[i], face, sizeof(bspfface_t)))
            return i;
    }
    
    bspfile.faces[bspfile.nfaces] = *face;
    return bspfile.nfaces++;
}

void FindFacePlanes()
{
    int i, j;
    
    bspfface_t *face;
    bspfplane_t *plane;
    
    vec3_t v1, v2, v3, a, b, n, rn;
    float d;
    
    for(i=0, face=bspfile.faces; i<bspfile.nfaces; i++, face++)
    {
        if(bspfile.surfedes[face->firstedge] >= 0)
            VectorCopy(v1, bspfile.vertices[bspfile.edges[bspfile.surfedes[face->firstedge]][0]]);
        else
            VectorCopy(v1, bspfile.vertices[bspfile.edges[-bspfile.surfedes[face->firstedge]][1]]);
        
        if(bspfile.surfedes[face->firstedge+1] >= 0)
            VectorCopy(v2, bspfile.vertices[bspfile.edges[bspfile.surfedes[face->firstedge+1]][0]]);
        else
            VectorCopy(v2, bspfile.vertices[bspfile.edges[-bspfile.surfedes[face->firstedge+1]][1]]);
        
        if(bspfile.surfedes[face->firstedge+2] >= 0)
            VectorCopy(v3, bspfile.vertices[bspfile.edges[bspfile.surfedes[face->firstedge+2]][0]]);
        else
            VectorCopy(v3, bspfile.vertices[bspfile.edges[-bspfile.surfedes[face->firstedge+2]][1]]);
        
        VectorSubtract(a, v2, v1);
        VectorSubtract(b, v3, v1);
        VectorCross(n, a, b);
        VectorNormalize(n, n);
        d = VectorDot(n, v1);
        
        for(j=0, plane=bspfile.planes; j<bspfile.nplanes; j++, plane++)
        {
            VectorMultiply(rn, plane->n, -1.0);
            if(VectorComp(n, plane->n) && d == plane->d)
            {
                face->plane = j;
                face->planeside = 0;
                break;
            }
            
            if(VectorComp(n, rn) && d == -plane->d)
            {
                face->plane = j;
                face->planeside = 1;
                break;
            }
        }
    }
}

void LoadModels()
{
    int i, j;
    
    bspmodel_t model;
    
    for(i=0; i<nmodels; i++)
    {
        memset(&model, 0, sizeof(bspmodel_t));
        model.firstface = bspfile.nfaces;
        for(j=0; j<NHULLS; j++)
        {
            model.rootnodes[j] = bspfile.nclipnodes;
            LoadNodes_r(&rootnode[i][j], !j);
        }
        model.nfaces = bspfile.nfaces - model.firstface;
        
        model.min[0] = bspfile.nodes[model.rootnodes[0]].min[0];
        model.min[1] = bspfile.nodes[model.rootnodes[0]].min[1];
        model.min[2] = bspfile.nodes[model.rootnodes[0]].min[2];
        model.max[0] = bspfile.nodes[model.rootnodes[0]].max[0];
        model.max[1] = bspfile.nodes[model.rootnodes[0]].max[1];
        model.max[2] = bspfile.nodes[model.rootnodes[0]].max[2];
        
        bspfile.models[bspfile.nmodels++] = model;
    }
}

splitplane_t MakeSplitNode(surfnode_t *surfs)
{
    splitplane_t newplane;
    surfnode_t *node, *curnode, *laston, *lastback, *lastfront, *newnode;
    surfnode_t *frontsurf, *backsurf;
    int sign;
    
    memset(&newplane, 0, sizeof(newplane));
    node = FindIdealSplitSurf(surfs);
    newplane.mainsurf = node;
    node->surf->onplane = true;
    VectorCopy(newplane.n, node->surf->n);
    newplane.d = node->surf->d;
    
    lastback = laston = lastfront = 0;
    for(curnode=surfs; curnode; curnode=curnode->next)
    {
        if(curnode->surf == node->surf)
            sign = 0;
        else
            sign = GetSurfSide(curnode->surf, newplane.n, newplane.d);
        
        newnode = (surfnode_t*) calloc(1, sizeof(surfnode_t));
        memcpy(newnode, curnode, sizeof(surfnode_t));
        newnode->next = newnode->last = 0;
        
        if(sign == -1)
        {
            if(lastback)
            {
                lastback->next = newnode;
                newnode->last = lastback;
                lastback = newnode;
            }
            else
            {
                lastback = newplane.childsurfs[0] = newnode;
            }
        }
        else if(sign == 1)
        {
            if(lastfront)
            {
                lastfront->next = newnode;
                newnode->last = lastfront;
                lastfront = newnode;
            }
            else
            {
                lastfront = newplane.childsurfs[1] = newnode;
            }
        }
        else if(sign == 0)
        {
            newnode->surf->onplane = true;
            if(laston)
            {
                laston->next = newnode;
                newnode->last = laston;
                laston = newnode;
            }
            else
            {
                laston = newplane.surfs = newnode;
            }
        }
        else if(sign == 2)
        {
            backsurf = newnode;
            frontsurf = (surfnode_t*) calloc(1, sizeof(surfnode_t));
            frontsurf->surf = CopySurf(backsurf->surf);

            ClipPoly(&backsurf->surf->geo, newplane.n, newplane.d, 0);
            ClipPoly(&frontsurf->surf->geo, newplane.n, newplane.d, 1);
            
            if(lastback)
            {
                lastback->next = newnode;
                newnode->last = lastback;
                lastback = newnode;
            }
            else
            {
                lastback = newplane.childsurfs[0] = newnode;
            }
            newnode = frontsurf;
            if(lastfront)
            {
                lastfront->next = newnode;
                newnode->last = lastfront;
                lastfront = newnode;
            }
            else
            {
                lastfront = newplane.childsurfs[1] = newnode;
            }
        }
        else
        {
            // Huh?
        }
    }
    
    return newplane;
}

void CutWorld_r(splitplane_t* parent)
{
    int i;
    
    boolean alldone;
    surfnode_t *cursurf, *unionsurfs, *lastunionsurf, *newsurf, *_cursurf;
    splitplane_t *newplane;
    
    for(i=0; i<2; i++)
    {
        newplane = (splitplane_t*)calloc(1, sizeof(splitplane_t));

        for(cursurf=parent->childsurfs[i], alldone = true; cursurf; cursurf=cursurf->next)
        {
            if(!cursurf->surf->onplane)
                alldone = false;
        }
        
        // All surfs on this side have been divided so its ok
        if(alldone)
        {
            newplane->leaf = calloc(1, sizeof(leaf_t));
            newplane->leaf->ileaf = nleaves++;
            newplane->leaf->surfs = parent->childsurfs[i];

            if(i==0) // Back
                newplane->leaf->contents = CONTENTS_SOLID;
            else     // Front
                newplane->leaf->contents = CONTENTS_EMPTY;

            parent->children[i] = newplane;
            continue;
        }
        
        lastunionsurf = unionsurfs = 0;
        for(_cursurf = parent->surfs; _cursurf; _cursurf=_cursurf->next)
        {
            newsurf = calloc(1, sizeof(surfnode_t));
            newsurf->surf = _cursurf->surf;
            newsurf->last = newsurf->next = 0;
            if(lastunionsurf)
            {
                lastunionsurf->next = newsurf;
                newsurf->last = lastunionsurf;
            }
            else
            {
                unionsurfs = newsurf;
            }
            
            lastunionsurf = newsurf;
        }

        for(_cursurf = parent->childsurfs[i]; _cursurf; _cursurf=_cursurf->next)
        {
            newsurf = calloc(1, sizeof(surfnode_t));
            newsurf->surf = _cursurf->surf;
            newsurf->last = newsurf->next = 0;
            if(lastunionsurf)
            {
                lastunionsurf->next = newsurf;
                newsurf->last = lastunionsurf;
            }
            else
            {
                unionsurfs = newsurf;
            }
            
            lastunionsurf = newsurf;
        }

        *newplane = MakeSplitNode(unionsurfs);
        newplane->parent = parent;
        for(_cursurf=unionsurfs; _cursurf; _cursurf=newsurf)
        {
            newsurf = _cursurf->next;
            free(_cursurf);
        }

        parent->children[i] = newplane;
        CutWorld_r(parent->children[i]);
    }
}

surfnode_t* FindIdealSplitSurf(surfnode_t *surfs)
{
    int bestscore, nfront, onpl, nback, score;
    surfnode_t *bestsurf;
    surfnode_t *curnode, *testnode;
    surf_t *cursurf, *testsurf;
    vec3_t a, b;
    float d;
    float sidesdiff, onscore;
    
    for(curnode=surfs, bestscore=-1; curnode; curnode=curnode->next)
    {
        cursurf = curnode->surf;
        if(cursurf->onplane)
            continue;
        
        // Figure out the surf's plane if it doesn't exist yet
        if(VectorComp(cursurf->n, vec3_origin))
        {
            VectorSubtract(a, cursurf->geo.first->val, cursurf->geo.first->next->val);
            VectorSubtract(b, cursurf->geo.first->val, cursurf->geo.first->next->next->val);
            VectorCross(cursurf->n, a, b);
            VectorNormalize(cursurf->n, cursurf->n);
            cursurf->d = VectorDot(cursurf->n, cursurf->geo.first->val);
        }
        
        for(testnode=surfs, nfront=onpl=nback=0; testnode; testnode=testnode->next)
        {
            if(testnode == curnode)
                continue;
            
            testsurf = testnode->surf;
            d = VectorDot(cursurf->n, testsurf->geo.first->val) - cursurf->d;
            
            if(d * d < 0.01)
                onpl++;
            else if(d < 0)
                nback++;
            else
                nfront++;
        }
        
        sidesdiff = (float) (nfront + nback - abs(nfront - nback)) / (float) (nfront + nback);
        onscore = (float) onpl / (float) (nfront + nback + onpl);
        
        score = onscore * 1000 + sidesdiff * 500;
        
        // Axially aligned planes are better
        if(cursurf->n[0] > 0.99 || cursurf->n[1] > 0.99 || cursurf->n[2] > 0.99)
            score += 300;
        
        if(score > bestscore)
        {
            bestscore = score;
            bestsurf = curnode;
        }
    }
    
    if(bestscore == -1)
        return 0;
    
    return bestsurf;
}

int GetSurfSide(surf_t* surf, vec3_t n, float d)
{
    int cursign, nfirst;
    float sign;
    vnode_t *curv;
    
    for(curv=surf->geo.first, nfirst=0, cursign=0;; curv=curv->next)
    {
        if(curv == surf->geo.first)
            nfirst++;
        if(nfirst > 1)
            break;
        
        sign = VectorDot(n, curv->val) - d;
        if (fabsf(sign) < 0.01)
            sign = 0;

        if(cursign == 0)
        {
            if(sign<0)
                cursign = -1;
            else if(sign>0)
                cursign = 1;
        }
        else if(sign * cursign < 0)
        {
            cursign = 2;
            return cursign;
        }
    }
    
    return cursign;
}

surf_t* CopySurf(surf_t* surf)
{
    int i;
    
    surf_t *newsurf;
    vnode_t *curv, *newv, *lastv;
    
    newsurf = malloc(sizeof(surf_t));
    memcpy(newsurf, surf, sizeof(surf_t));
    
    for(curv=surf->geo.first, i=0, lastv=0;; curv=curv->next)
    {
        if(curv==surf->geo.first)
            i++;
        if(i>1)
            break;
        
        newv = AllocVert();
        memcpy(newv, curv, sizeof(vnode_t));
        
        if(lastv)
        {
            lastv->next = newv;
            newv->last = lastv;
            newsurf->geo.first->last = newv;
            newv->next = newsurf->geo.first;
            lastv = newv;
        }
        else
        {
            lastv = newv;
            newv->next = newv->last = newv;
            newsurf->geo.first = newv;
        }
    }
    
    return newsurf;
}

void FreeSurfnodes(surfnode_t* list)
{
    surfnode_t* nextnode;

    for(; list; list=nextnode)
    {
        nextnode = list->next;
        list->surf->onplane = list->surf->marked = false;
        free(list);
    }
}

void FreeTree_r(splitplane_t* headnode, void*** freedportals, int* nfreedportals, int depth)
{
    int i;
    portalnode_t* p;
    portalnode_t* nextp;

    portal_t* prt;

    if(headnode->leaf)
    {
        for(p=headnode->portals; p; p=nextp)
        {
            nextp = p->next;
            free(p);
        }

        free(headnode->leaf);
        free(headnode);
        return;
    }

    for(i=0; i<2; i++)
        FreeTree_r(headnode->children[i], freedportals, nfreedportals, depth + 1);

    // DON'T free mainsurf because it lives in surfs
    FreeSurfnodes(headnode->surfs);
    for(i=0; i<2; i++)
        FreeSurfnodes(headnode->childsurfs[i]);

    for(p=headnode->portals; p; p=nextp)
    {
        nextp = p->next;
        prt = p->p;
        free(p);

        for(i=0; i<(*nfreedportals); i++)
        {
            if((*freedportals)[i] == (void*) prt)
                break;
        }

        if(i < (*nfreedportals))
            continue;

        if(!(*nfreedportals))
            (*freedportals) = malloc(sizeof(void*) * (*nfreedportals + 1));
        else
            (*freedportals) = realloc((*freedportals), sizeof(void*) * (*nfreedportals + 1));

        (*freedportals)[(*nfreedportals)++] = prt;
        free(prt);
    }

    if(depth)
        free(headnode);
}

void FreeTree(splitplane_t* headnode)
{
    int nfreedportals = 0;
    void** freedportals = 0;

    FreeTree_r(headnode, &freedportals, &nfreedportals, 0);
    free(freedportals);
}