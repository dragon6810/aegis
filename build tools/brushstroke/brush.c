//
//  brush.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/24/24.
//

#include "brush.h"

#include "mapfile.h"

void AddPlane(brushdef_t* brsh, planedef_t pl)
{
    planedef_t* ppl;
    
    ppl = (planedef_t*) malloc(sizeof(planedef_t));
    memcpy(ppl, &pl, sizeof(planedef_t));
    
    if(!brsh->firstpl)
        brsh->firstpl->next = brsh->firstpl = brsh->lastpl = ppl;
    else
        brsh->lastpl = brsh->lastpl->next = ppl;
    
    brsh->nplanes++;
}

void GenPolys(brushdef_t* brsh)
{
    int i;
    
    planedef_t* pl;
    polynode_t* p;
    
    for(i=0, pl=brsh->firstpl, p=brsh->firstp; pl; i++, pl=pl->next, p++)
    {
        p = (polynode_t*) malloc(sizeof(polynode_t));
        HungryPoly(p, pl->n, pl->d);
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
    int i, j;
    
    polynode_t* p;
    planedef_t* pl;
    vnode_t* v;
    
    for(i=0, p=brsh->firstp; p; i++, p=p->next)
    {
        for(j=0, pl=brsh->firstpl; j<brsh->nplanes; j++, pl=pl->next)
        {
            if(pl == p->pl) // Don't clip yourself against your own plane!
                continue;
            
            ClipPoly(p, pl->n, pl->d, 0);
        }
    }
}

void GenBB(brushdef_t* brsh)
{
    int i, j;
    
    boolean started;
    polynode_t *p;
    vnode_t* v;
    
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
            goto skipbr;
        
        for(i=0; i<3; i++)
        {
            if (br1->bbmin[i] > br2->bbmax[i] || br1->bbmax[i] < br2->bbmin[i])
                goto skipbr;
        }
        
        for(pl=br2->firstpl; pl; pl=pl->next)
        {
            for(p=br1->firstp; p; p=nextp)
            {
                nextp = p->next;
                
                if(SlicePoly(p, pl->n, pl->d))
                {
                    if(PolyInsideBrush(p->next, br2))
                        CullPoly(p->next, br1);
                }
                if(PolyInsideBrush(p, br2))
                    CullPoly(p, br1);
            }
        }
        
    skipbr:
        i=0;
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
