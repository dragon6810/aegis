//
//  polygon.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/21/24.
//

#include "polygon.h"
#include "mapdefs.h"
#include "vector.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

vnode_t* AllocVert()
{
    return (vnode_t*) malloc(sizeof(vnode_t));
}

polynode_t* AllocPoly()
{
    return (polynode_t*) malloc(sizeof(polynode_t));
}

void HungryPoly(polynode_t* poly, vec3_t n, float d)
{
    int i;
    
    vec3_t points[6];
    int npoints;
    vec3_t v1, v2, p;
    float d1, d2, t;
    
    vnode_t *cur, *first, *last;

    for(i=0, npoints=0; i<12; i++)
    {
        switch(i)
        {
            case 0:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = -MAP_MAX_BOUNDS; v2[2] = -MAP_MAX_BOUNDS;
                break;
            case 1:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = -MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            case 2:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = -MAP_MAX_BOUNDS;
                break;
            case 3:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = MAP_MAX_BOUNDS; v1[2] = MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            case 4:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = -MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = -MAP_MAX_BOUNDS;
                break;
            case 5:
                v1[0] = MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = -MAP_MAX_BOUNDS;
                break;
            case 6:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = MAP_MAX_BOUNDS;
                v2[0] = -MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            case 7:
                v1[0] = MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            case 8:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = -MAP_MAX_BOUNDS; v2[1] = -MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            case 9:
                v1[0] = MAP_MAX_BOUNDS; v1[1] = -MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = -MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            case 10:
                v1[0] = -MAP_MAX_BOUNDS; v1[1] = MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = -MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            case 11:
                v1[0] = MAP_MAX_BOUNDS; v1[1] = MAP_MAX_BOUNDS; v1[2] = -MAP_MAX_BOUNDS;
                v2[0] = MAP_MAX_BOUNDS; v2[1] = MAP_MAX_BOUNDS; v2[2] = MAP_MAX_BOUNDS;
                break;
            default:
                break;
        }
        
        
        d1 = VectorDot(v1, n) - d;
        d2 = VectorDot(v2, n) - d;
        
        if(d1 == d2 || d1 * d2 > 0)
            continue;
        
        t = d1 / (d1 - d2);
        
        
        // Lerp
        VectorSubtract(p, v2, v1);
        VectorMultiply(p, p, t);
        VectorAdd(p, p, v1);
        
        
        VectorCopy(points[npoints], p);
        
        npoints++;
    }
    
    first = last = 0;
    for(i=0; i<npoints; i++)
    {
        cur = malloc(sizeof(vnode_t));
        VectorCopy(cur->val, points[i]);
        
        if(!first)
        {
            poly->first = first = last = cur;
            first->next = first->last = cur;
        }
        else
        {
            cur->last = last;
            last->next = cur;
            last = cur;
        }
    }
    
    last->next = first;
    first->last = last;
    poly->first = first;
    VectorCopy(poly->normal, n);
    WindPoly(poly);
}

void WindPoly(polynode_t* poly)
{
    int i;
    
    int nfirst;
    int npoints;
    
    vec3_t right, up, center, p;
    float x, y;
    
    vnode_t* node, *next, *end;
    
    up[0] = 0; up[1] = 0; up[2] = 1;
    VectorCross(right, poly->normal, up);
    if(VectorLengthSqr(right) < 0.1)
    {
        up[0] = 0; up[1] = 1; up[2] = 0;
        VectorCross(right, poly->normal, up);
    }
    VectorCross(up, poly->normal, right);
    
    center[0] = 0; center[1] = 0; center[2] = 0;
    for(nfirst=0, npoints=0, node=poly->first; nfirst<2; node=node->next, npoints++)
    {
        if(node == poly->first)
            nfirst++;
        if(nfirst>1)
            break;
        
        VectorAdd(center, center, node->val);
    }
    VectorDivide(center, center, (float) npoints);
    
    vnode_t* nodes[npoints];
    for(nfirst=0, npoints=0, node=poly->first; nfirst<2; node=next, npoints++)
    {
        if(node == poly->first)
            nfirst++;
        if(nfirst>1)
            break;
        
        nodes[npoints] = (vnode_t*) malloc(sizeof(vnode_t));
        
        next=node->next;
        VectorSubtract(p, node->val, center);
        x = VectorDot(right, p);
        y = VectorDot(up, p);
        
        memcpy(nodes[npoints], node, sizeof(vnode_t));
        nodes[npoints]->a = atan2f(y, x);
    }
    
    qsort(nodes, npoints, sizeof(vnode_t*), CompNodes);
    for(i=0; i<npoints; i++)
    {
        nodes[i]->next = nodes[(i + 1) % npoints];
        nodes[i]->last = nodes[(i - 1 + npoints) % npoints];
    }
    
    node = poly->first;
    end = node;
    
    poly->first = nodes[0];
    
    do
    {
        next = node->next;
        free(node);
        node = next;
    } while (node != end);
}

void ClipPoly(polynode_t* poly, vec3_t n, float d)
{
    int nfirst;
    boolean infirst;
    vnode_t *node, *last, *fout, *fin, *next, *new, *finlast;
    vec3_t delta;
    float dlast, dcur, t;
    
    infirst = false;
    for(nfirst=fout=fin=0, node=poly->first; nfirst<2; node=node->next)
    {
        if(node == poly->first)
            nfirst++;
        if(nfirst>1)
            break;
        
        if(fin && fout)
            break;
        
        last = node->last;
        
        dlast = VectorDot(n, last->val) - d;
        dcur = VectorDot(n, node->val) - d;
        
        if(dlast * dcur >= 0)
            continue;
        
        if(dlast < 0 && dcur > 0) // Moving in front of plane
            fout = node;
        else // Moving behind plane
        {
            if(!fout)
                infirst = true;
            fin = node->last;
        }
    }
    
    if(!fin || !fout) // Polygon is fully behind/in front of the plane
        return;
    
    if(fin == fout)
    {
        new = (vnode_t*) malloc(sizeof(vnode_t));
        VectorCopy(new->val, fout->val);
        new->next = fout->next;
        new->last = fout;
        fout->next = new;
    }
    else
    {
        if(infirst)
        {
            for(node=fin->last; node!=fout; node=next)
            {
                next = node->last;
                ClipVert(node, poly);
            }
        }
        else
        {
            for(node=fout->next; node!=fin; node=next)
            {
                next = node->next;
                ClipVert(node, poly);
            }
        }
    }
    
    // Snap first out to plane
    last = fout->last;
    node = fout;
    
    VectorSubtract(delta, node->val, last->val);
    t = (d - VectorDot(n, last->val)) / VectorDot(n, delta);
    VectorMultiply(delta, delta, t);
    VectorAdd(node->val, last->val, delta);
    
    // Snap first in to plane
    last = fin->next;
    node = fin;
    
    VectorSubtract(delta, node->val, last->val);
    float a = VectorDot(n, last->val);
    float b = VectorDot(n, node->val);
    t = (d - VectorDot(n, last->val)) / VectorDot(n, delta);
    VectorMultiply(delta, delta, t);
    VectorAdd(node->val, last->val, delta);
    
    //free(finlast);
}

void ClipVert(vnode_t* v, polynode_t* p)
{
    if(v == p->first)
        p->first = v->next;
    
    v->last->next = v->next;
    v->next->last = v->last;
    
    free(v);
}

int CompNodes(const void* a, const void* b)
{
    vnode_t* nodeA = *(vnode_t**)a;
    vnode_t* nodeB = *(vnode_t**)b;
        
    if (nodeA->a < nodeB->a) return -1;
    if (nodeA->a > nodeB->a) return 1;
    return 0;
}
