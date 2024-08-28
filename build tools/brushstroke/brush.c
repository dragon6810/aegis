//
//  brush.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/24/24.
//

#include "brush.h"

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
    
    brsh->npolys = brsh->nplanes;
    brsh->firstp = malloc(brsh->npolys * sizeof(polynode_t));
    
    for(i=0, pl=brsh->firstpl, p=brsh->firstp; i<brsh->npolys; i++, pl=pl->next, p++)
    {
        HungryPoly(p, pl->n, pl->d);
        p->pl = pl;
    }
}

void CutPolys(brushdef_t* brsh)
{
    int i, j;
    
    polynode_t* p;
    planedef_t* pl;
    vnode_t* v;
    
    for(i=0, p=brsh->firstp; i<brsh->npolys; i++, p++)
    {
        for(j=0, pl=brsh->firstpl; j<brsh->nplanes; j++, pl=pl->next)
        {
            if(pl == p->pl) // Don't clip yourself against your own plane!
                continue;
            
            ClipPoly(p, pl->n, pl->d);
        }
    }
    
    printf("Cut polygons of brush:\n");
    for(i=0, p=brsh->firstp; i<brsh->npolys; i++, p++)
    {
        printf("Polygon %d:\n", i);
        for(j=0, v=p->first; j<2; v=v->next)
        {
            if(v == p->first)
                j++;
            if(j>1)
                break;
            
            printf("(%d %d %d) ", (int)v->val[0], (int)v->val[1], (int)v->val[2]);
        }
        printf("\n");
    }
}
