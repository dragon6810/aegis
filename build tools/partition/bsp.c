//
//  bsp.c
//  Partition
//
//  Created by Henry Dunn on 9/18/24.
//

#include "map.h"

void LoadBrushSets(char* file)
{
    int i, j;
    
    int modelnum;
    surf_t *newsurf;
    brushset_t *newset;
    brushsetnode_t *newsetnode, *lastsetnode;
    surfnode_t *newsurfnode, *lastsurfnode;
    vnode_t *newv, *lastv, *firstv;
    vec3_t val;
    
    char *newfile;
    
    filename = file;
    
    newfile = malloc(strlen(file) + 3);
    memset(newfile, 0, strlen(file) + 3);
    memcpy(newfile, file, strlen(file));
    newfile[strlen(file) + 0] = '.';
    newfile[strlen(file) + 1] = 'g';
    for(i=0; i<NHULLS; i++)
    {
        newfile[strlen(file) + 2] = '0' + i;
        gfiles[i] = fopen(newfile, "r");
        if(!gfiles[i])
        {
            printf("File does not exist %s!\n", newfile);
            exit(1);
        }
        
        lastsetnode = 0;
        while(!feof(gfiles[i]))
        {
            if(fscanf(gfiles[i], " *%d\n", &modelnum))
            {
                newset = (brushset_t*) malloc(sizeof(brushset_t));
                newsetnode = (brushsetnode_t*) malloc(sizeof(brushsetnode_t));
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
                while(j != 0)
                {
                    newsurf = malloc(sizeof(surf_t));
                    j = lastv = 0;
                    while(fscanf(gfiles[i], "( %f %f %f )", &val[0], &val[1], &val[2]) == 3)
                    {
                        newv = malloc(sizeof(vnode_t));
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
                    newsurf->geo.first = firstv;
                    
                    fscanf(gfiles[i], "[ %f %f %f %d ] ", &newsurf->s[0], &newsurf->s[1], &newsurf->s[2], &newsurf->sshift);
                    fscanf(gfiles[i], "[ %f %f %f %d ] ", &newsurf->t[0], &newsurf->t[1], &newsurf->t[2], &newsurf->tshift);
                    fscanf(gfiles[i], "%s\n", newsurf->texname);
                    
                    newsurfnode = (surfnode_t*) malloc(sizeof(surfnode_t));
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
                }
            }
            else
            {
                return;
            }
        }
    }
    
    free(newfile);
}

void ProcessWorld()
{
    // Chid index doesnt matter for root
    rootnode = MakeSplitNode(brushsets[0]->brushet->firstsurf);
    CutWorld_r(&rootnode);
}

splitplane_t MakeSplitNode(surfnode_t *surfs)
{
    splitplane_t newplane;
    surfnode_t *node, *curnode, *laston, *lastback, *lastfront, *newnode;
    surfnode_t *frontsurf, *backsurf;
    int sign;
    
    memset(&newplane, 0, sizeof(newplane));
    node = FindIdealSplitSurf(surfs);
    node->surf->onplane = true;
    VectorCopy(newplane.n, node->surf->n);
    newplane.d = node->surf->d;
    
    lastback = laston = lastfront = 0;
    for(curnode=surfs; curnode; curnode=curnode->next)
    {
        sign = GetSurfSide(curnode->surf, newplane.n, newplane.d);
        newnode = (surfnode_t*) malloc(sizeof(surfnode_t));
        memcpy(newnode, curnode, sizeof(surfnode_t));
        newnode->next = newnode->last = 0;
        
        if(sign == -1)
        {
            if(lastback)
            {
                lastback->next = newnode;
                newnode->last = lastback;
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
            }
            else
            {
                laston = newplane.surfs = newnode;
            }
        }
        else if(sign == 2)
        {
            backsurf = newnode;
            frontsurf = (surfnode_t*) malloc(sizeof(surfnode_t));
            memcpy(frontsurf, backsurf, sizeof(surfnode_t));
            frontsurf->surf = CopySurf(backsurf->surf);
            
            ClipPoly(&backsurf->surf->geo, newplane.n, newplane.d, 0);
            ClipPoly(&frontsurf->surf->geo, newplane.n, newplane.d, 1);
            
            if(lastback)
            {
                lastback->next = newnode;
                newnode->last = lastback;
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
    surfnode_t *cursurf;
    splitplane_t *newplane;
    
    for(i=0; i<2; i++)
    {
        for(cursurf=parent->childsurfs[i], alldone = true; cursurf; cursurf=cursurf->next)
        {
            if(!cursurf->surf->onplane)
                alldone = false;
        }
        
        // All surfs on this side have been divided so its ok
        if(alldone)
            continue;
        
        newplane = (splitplane_t*) malloc(sizeof(splitplane_t));
        *newplane = MakeSplitNode(parent->childsurfs[i]);
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
    
    for(curv=newsurf->geo.first, i=lastv=0;; curv=curv->next)
    {
        if(curv==newsurf->geo.first)
            i++;
        if(i>1)
            break;
        
        newv = (vnode_t*) malloc(sizeof(vnode_t));
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
