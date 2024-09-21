//
//  bsp.c
//  Partition
//
//  Created by Henry Dunn on 9/18/24.
//

#include "map.h"

void LoadBrushSets(char* file)
{
    int i;
    
    int modelnum;
    surf_t *newsurf;
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
        
        while(!feof(gfiles[i]))
        {
            if(fscanf(gfiles[i], " *%d\n", &modelnum))
            {
                while(!fscanf(gfiles[i], " *%d\n", &modelnum))
                {
                    newsurf = malloc(sizeof(surf_t));
                    while(fscanf(gfiles[i], "(%5.3f %5.3f %5.3f) ", &val[0], &val[1], &val[2]))
                    {
                        newv = malloc(sizeof(vnode_t));
                        VectorCopy(newv->val, val);
                        if(lastv)
                        {
                            newv->last = lastv;
                            newv->next = firstv;
                            firstv->next = newv;
                            lastv->next = newv;
                        }
                        else
                        {
                            firstv = newv;
                            newv->next = newv->last = newv;
                        }
                        
                        lastv = newv;
                    }
                    newsurf->geo.first = firstv;
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

surf_t FindIdealSplitSurf(surfnode_t *surfs)
{
    int bestscore;
    surfnode_t *bestsurf;
    surfnode_t *curnode;
    surf_t *cursurf;
    
}
