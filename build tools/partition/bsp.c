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
    surfnode_t *newnode, *prevnode, *newlist;
    vnode_t *newv, *lastv, *firstv;
    vec3_t val, s, t;
    int sshift, tshift;
    char texname[17];
    
    char *newfile;
    
    filename = file;
    
    newfile = malloc(strlen(file) + 3);
    memset(newfile, 0, strlen(file) + 3);
    memcpy(newfile, file, strlen(file));
    newfile[strlen(file) + 0] = '.';
    newfile[strlen(file) + 1] = 'g';
    for(i=0; i<NHULLS; i++)
    {
        newlist = 0;

        newfile[strlen(file) + 2] = '0' + i;
        newfile[strlen(file) + 3] = 0;
        gfiles[i] = fopen(newfile, "r");
        if(!gfiles[i])
        {
            printf("File does not exist %s!\n", newfile);
            free(newfile);
            exit(1);
        }
        
        while(!feof(gfiles[i]))
        {
            if(fscanf(gfiles[i], " *%d\n", &modelnum))
            {
                while(!fscanf(gfiles[i], " *%d\n", &modelnum))
                {
                    lastv = firstv = prevnode = 0;

                    newsurf = malloc(sizeof(surf_t));
                    while(fscanf(gfiles[i], "( %f %f %f )", &val[0], &val[1], &val[2]))
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

                    if (!fscanf(gfiles[i], "[ %f %f %f %d ] [ %f %f %f %d ] %s\n", &s[0], &s[1], &s[2], &sshift, &t[0], &t[1], &t[2], &tshift, texname))
                    {
                        free(newsurf);
                        break;
                    }

                    VectorCopy(newsurf->s, s);
                    VectorCopy(newsurf->t, t);
                    newsurf->sshift = sshift;
                    newsurf->tshift = tshift;
                    memcpy(newsurf->texname, texname, sizeof(texname));

                    newnode = malloc(sizeof(surfnode_t));
                    newnode->surf = newsurf;
                    if (prevnode)
                    {
                        prevnode->next = newnode;
                        newnode->last = prevnode;
                    }

                    if (!newlist)
                        newlist = newnode;

                    prevnode = newnode;
                }
            }
            else
            {
                return;
            }
        }

        brushsets[i] = newlist;
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
