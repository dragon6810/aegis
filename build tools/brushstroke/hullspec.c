//
//  hullspec.c
//  Brushstroke
//
//  Created by Henry Dunn on 9/12/24.
//

#include "hullspec.h"

vec3_t hmins[4] = { { 0, 0, 0 }, { -16, -16, -36 }, { -32, -32, -32 }, { -16, -16, -18 } };
vec3_t hmaxs[4] = { { 0, 0, 0 }, { 16, 16, 36 }, { 32, 32, 32 }, { 16, 16, 18 } };

void ReadHullSpec(char* path)
{
    int i;
    
    int imin[3], imax[3];
    vec3_t min, max;
    
    hullptr = fopen(path, "r");
    if(!hullptr)
    {
        printf("WARNING: Bad hull specification path: file does not exist!\n");
        return;
    }
    
    fseek(hullptr, 2, SEEK_CUR);
    
    for(i=0; i<NHULLS; i++)
    {
        if(fscanf(hullptr, "(%d %d %d) (%d %d %d)\n", &imin[0], &imin[1], &imin[2], &imax[0], &imax[1], &imax[2]) != 6)
        {
            printf("WARNING: Bad hull specification given!\n");
            return;
        }
        
        min[0] = (float) imin[0]; min[1] = (float) imin[1]; min[2] = (float) imin[2];
        max[0] = (float) imax[0]; max[1] = (float) imax[1]; max[2] = (float) imax[2];
        
        VectorCopy(hmins[i], min);
        VectorCopy(hmaxs[i], max);
    }
    
    fclose(hullptr);
}
