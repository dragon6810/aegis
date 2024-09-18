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
