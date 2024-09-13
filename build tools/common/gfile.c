//
//  gfile.c
//  Brushstroke
//
//  Created by Henry Dunn on 9/10/24.
//

#include "gfile.h"

void WritePoly(polynode_t p, FILE* ptr)
{
    int i;
    
    vnode_t* v;
    
    for(v=p.first, i=0;; v=v->next)
    {
        if(v==p.first)
            i++;
        if(i>1)
            break;
        
        fprintf(ptr, "( %5.3f %5.3f %5.3f )", v->val[0], v->val[1], v->val[2]);
    }
    
    fprintf(ptr, "[ %5.3f %5.3f %5.3f %d ] ", p.pl->shat[0], p.pl->shat[1], p.pl->shat[2], (int) p.pl->sshift);
    fprintf(ptr, "[ %5.3f %5.3f %5.3f %d ] ", p.pl->that[0], p.pl->that[1], p.pl->that[2], (int) p.pl->tshift);
    fprintf(ptr, "%s\n", p.pl->texname);
}
