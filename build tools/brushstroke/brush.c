//
//  brush.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/24/24.
//

#include "brush.h"

#include "mapfile.h"
#include <math.h>

void AddPlane(brushdef_t* brsh, planedef_t pl)
{
    planedef_t* ppl;
    
    ppl = (planedef_t*) malloc(sizeof(planedef_t));
    memcpy(ppl, &pl, sizeof(planedef_t));
    ppl->next = 0;
    
    if(!brsh->firstpl)
        brsh->firstpl->next = brsh->firstpl = brsh->lastpl = ppl;
    else
        brsh->lastpl = brsh->lastpl->next = ppl;
    
    brsh->nplanes++;
}

void GenPolys(brushdef_t* brsh)
{
    int i, j, k;
    
    planedef_t *pl, *_pl, *nextpl, *lastpl;
    polynode_t* p;
    vnode_t *original, *cur, *v;
    
    // Prune duplicate faces often generatted when expanding hulls
    for(pl=brsh->firstpl; pl; pl=pl->next)
    {
        for(_pl=brsh->firstpl, lastpl=0; _pl; lastpl=_pl, _pl=nextpl)
        {
            nextpl = _pl->next;
            if(pl == _pl)
                continue;
            
            if(fabs(pl->d - _pl->d) > 0.01)
                continue;
            
            if(!VectorComp(pl->n, _pl->n))
                continue;
            
            if(lastpl)
                lastpl->next = _pl->next;
            else
                brsh->firstpl = _pl->next;
            
            brsh->nplanes--;
            free(_pl);
        }
    }
    
    for(i=0, pl=brsh->firstpl, p=brsh->firstp; pl; i++, pl=pl->next, p++)
    {
        p = (polynode_t*) malloc(sizeof(polynode_t));
        HungryPoly(p, pl->n, pl->d);
        ValidatePoly(p);
        p->pl = pl;
        if(!brsh->firstp)
            brsh->firstp = brsh->lastp = p;
        else
        {
            p->last = brsh->lastp;
            brsh->lastp->next = p;
            brsh->lastp = p;
        }
    }
}

void CutPolys(brushdef_t* brsh)
{
    polynode_t* p, *nextp;
    planedef_t* pl;
    
    for(p=brsh->firstp; p; p=nextp)
    {
        for(pl=brsh->firstpl; pl; pl=pl->next)
        {
            if(pl == p->pl) // Don't clip yourself against your own plane!
                continue;
            
            nextp = p->next;
            ClipPoly(p, pl->n, pl->d, 0);
            if(!p->first)
            {
                CullPlane(p->pl, brsh);
                CullPoly(p, brsh);
                break; // Break outta the plane loop because this poly is gone
            }
        }
    }
}

void GenBB(brushdef_t* brsh)
{
    int i, j;
    
    boolean started;
    polynode_t *p;
    vnode_t* v;
    planedef_t *a, *b, *c;
    float det, dx, dy, dz;
    vec3_t in;
    
    for(a=brsh->firstpl; a; a=a->next)
    {
        for(b=brsh->firstpl; b; b=b->next)
        {
            if(a==b)
                continue;
            for(c=brsh->firstpl; c; c=c->next)
            {
                if(b==c)
                    continue;
                
                det = a->n[0] * (b->n[1] * c->n[2] - b->n[2] * c->n[1]) -
                      a->n[1] * (b->n[0] * c->n[2] - b->n[2] * c->n[0]) +
                      a->n[2] * (b->n[0] * c->n[1] - b->n[1] * c->n[0]);
                
                if(det == 0)
                    continue;
                
                dx = a->d * (b->n[1] * c->n[2] - b->n[2] * c->n[1]) -
                    a->n[1] * (b->d * c->n[2] - b->n[2] * c->d) +
                    a->n[2] * (b->d * c->n[1] - b->n[1] * c->d);

                dy = a->n[0] * (b->d * c->n[2] - b->n[2] * c->d) -
                    a->d * (b->n[0] * c->n[2] - b->n[2] * c->n[0]) +
                    a->n[2] * (b->n[0] * c->d - b->d * c->n[0]);

                dz = a->n[0] * (b->n[1] * c->d - b->d * c->n[1]) -
                    a->n[1] * (b->n[0] * c->d - b->d * c->n[0]) +
                    a->d * (b->n[0] * c->n[1] - b->n[1] * c->n[0]);
                
                in[0] = dx / det;
                in[1] = dy / det;
                in[2] = dz / det;
                
                if(!started)
                {
                    VectorCopy(brsh->bbmin, in);
                    VectorCopy(brsh->bbmax, in);
                    started=true;
                    continue;
                }
                
                for(j=0; j<3; j++)
                {
                    if(in[j] < brsh->bbmin[j])
                        brsh->bbmin[j] = in[j];
                    if(in[j] > brsh->bbmax[j])
                        brsh->bbmax[j] = in[j];
                }
            }
        }
    }
    
    /*
    for(i=0, started=false, p=brsh->firstp; p; i++, p=p->next)
    {
        for(v=p->first->next; v!=p->first; v=v->next)
        {
            if(!started)
            {
                VectorCopy(brsh->bbmin, v->val);
                VectorCopy(brsh->bbmax, v->val);
                started=true;
                continue;
            }
            
            for(j=0; j<3; j++)
            {
                if(v->val[j] < brsh->bbmin[j])
                    brsh->bbmin[j] = v->val[j];
                if(v->val[j] > brsh->bbmax[j])
                    brsh->bbmax[j] = v->val[j];
            }
        }
    }
     */
}

/*
    Optimize:
    Will slice and cull faces of brushes inside other brushes for optimizations sake.
    Partition will later cull unreachable faces, so ideally at the end of this only
    reachable parts of a surface will exist. (Hopefully);
*/
void Optimize(brushdef_t* brsh, entitydef_t* set)
{
    int i;
    
    brushdef_t *br1, *br2;
    polynode_t *p, *nextp;
    planedef_t *pl;
    
    // Go until its a nullptr (end of linked list)
    br1=brsh;
    for(br2=set->firstbrsh; br2; br2=br2->next)
    {
        if(br1==br2) // Don't clip against yourself!
            continue;
        
        for(i=0; i<3; i++)
        {
            if (br1->bbmin[i] > br2->bbmax[i] || br1->bbmax[i] < br2->bbmin[i])
                break;
        }
        
        if(i < 3)
            continue;
        
        for(pl=br2->firstpl; pl; pl=pl->next)
        {
            for(p=br1->firstp, i=0; p; p=nextp, i++)
            {
                nextp = p->next;
                
                if(SlicePoly(p, pl->n, pl->d))
                {
                    if(p == br1->lastp)
                        br1->lastp = p->next;
                    
                    if(PolyInsideBrush(p->next, br2))
                        CullPoly(p->next, br1);
                }
                
                if(PolyInsideBrush(p, br2))
                    CullPoly(p, br1);
                
                if(i>999)
                    printf("Uh oh\n");
            }
        }
    }
}

boolean PolyInsideBrush(polynode_t* p, brushdef_t* brsh)
{
    planedef_t* pl;
    
    for(pl=brsh->firstpl; pl; pl=pl->next)
    {
        if(!PolyInsidePlane(p, pl->n, pl->d))
            return false;
    }
    
    return true;
}

void CullPoly(polynode_t* p, brushdef_t* owner)
{
    if(p == owner->firstp)
        owner->firstp = p->next;
    if(p == owner->lastp)
        owner->lastp = p->last;
    
    if(p->last)
        p->last->next = p->next;
    if(p->next)
        p->next->last = p->last;
    free(p);
}

void CullPlane(planedef_t* p, brushdef_t* owner)
{
    planedef_t* curpl;
    
    if(p == owner->firstpl)
        owner->firstpl = p->next;
    if(p == owner->lastpl)
    {
        for(curpl=owner->firstpl; curpl->next->next; curpl=curpl->next);
        owner->lastpl = curpl;
    }
    
    for(curpl=owner->firstpl; curpl->next!=p; curpl=curpl->next);
    if(curpl)
        curpl->next = p->next;
    free(p);
    owner->nplanes--;
}

vnode_t* GetPoints(brushdef_t *brsh)
{
    int i, j;
    
    boolean started;
    polynode_t *p;
    vnode_t *firstv, *lastv, *v;
    planedef_t *a, *b, *c;
    float det, dx, dy, dz;
    vec3_t in;
    
    firstv = lastv = 0;
    for(a=brsh->firstpl; a; a=a->next)
    {
        for(b=brsh->firstpl; b; b=b->next)
        {
            if(a==b)
                continue;
            for(c=brsh->firstpl; c; c=c->next)
            {
                if(b==c || b==a)
                    continue;
                
                det = a->n[0] * (b->n[1] * c->n[2] - b->n[2] * c->n[1]) -
                      a->n[1] * (b->n[0] * c->n[2] - b->n[2] * c->n[0]) +
                      a->n[2] * (b->n[0] * c->n[1] - b->n[1] * c->n[0]);
                
                if(det == 0)
                    continue;
                
                dx = a->d * (b->n[1] * c->n[2] - b->n[2] * c->n[1]) -
                    a->n[1] * (b->d * c->n[2] - b->n[2] * c->d) +
                    a->n[2] * (b->d * c->n[1] - b->n[1] * c->d);

                dy = a->n[0] * (b->d * c->n[2] - b->n[2] * c->d) -
                    a->d * (b->n[0] * c->n[2] - b->n[2] * c->n[0]) +
                    a->n[2] * (b->n[0] * c->d - b->d * c->n[0]);

                dz = a->n[0] * (b->n[1] * c->d - b->d * c->n[1]) -
                    a->n[1] * (b->n[0] * c->d - b->d * c->n[0]) +
                    a->d * (b->n[0] * c->n[1] - b->n[1] * c->n[0]);
                
                in[0] = dx / det;
                in[1] = dy / det;
                in[2] = dz / det;
                
                v = malloc(sizeof(vnode_t));
                memset(v, 0, sizeof(v));
                VectorCopy(v->val, in);
                
                if(firstv)
                {
                    lastv->next = v;
                    v->last = lastv;
                    lastv = v;
                }
                else
                    lastv = firstv = v;
            }
        }
    }
    
    // Don't cyclically link
    return firstv;
}
