//
//  mapfile.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/22/24.
//

#include "mapfile.h"

int curhull = 0;
int linenum = 0;
entitydef_t *firstent, *lastent;

void ParseMap(char* name)
{
    filename = name;
    
    printf("Parsing map \"%s\"...\n", name);
    
    mapfile = fopen(name, "r");
    if(!mapfile)
        Error("Bad map file path");
    
    // TODO: Fix this no good badd quick-fix way of doing things and don't make everything reload for each hull!
    for(;curhull<NHULLS; curhull++)
    {
        char* hullname = malloc(strlen(filename));
        memset(hullname, 0, strlen(filename));
        memcpy(hullname, filename, strlen(filename) - 3);
        hullname[strlen(hullname)] = 'g';
        hullname[strlen(hullname)] = '0' + curhull;
        hullouts[curhull] = fopen(hullname, "w");
        free(hullname);
        
        linenum = 0;
        fseek(mapfile, 0, SEEK_SET);
        NextLine();
        while(ParseEntry());
        Finish();
        MemClean();
        
        fclose(hullouts[curhull]);
    }
}

boolean ParseEntry()
{
    char c;
    void* entry;
    entitydef_t *ent;
    
    if(strcmp(line, "{"))
        return false;
    
    ent = ParseEntity();
    if(ent)
    {
        if(firstent)
            lastent = lastent->next = ent;
        else
            firstent = lastent = ent;
    }
    
    return true;
}

entitydef_t* ParseEntity()
{
    entitydef_t* ent;
    brushdef_t* brsh;
    entitypair_t* pair, *lastpair;
    char key[MAX_KEY+1];
    char val[MAX_VALUE+1];
    
    ent = AllocEntity();
    memset(ent, 0, sizeof(entitydef_t));
    
    do
    {
        NextLine();
        if(!strcmp(line, "}"))
            break;
        
        memset(key, 0, sizeof(key));
        memset(val, 0, sizeof(val));
        
        if(sscanf(line, "\"%[^\"]\" \"%[^\"]\"", key, val) != 2)
        {
            if(strcmp(line, "{"))
            {
                printf("Error on line %d: \"%s\"\n", linenum, line);
                Error("Mapfile Syntax Error");
            }
            
            brsh = ParseBrush();
            if(!ent->firstbrsh)
                ent->firstbrsh = ent->lastbrsh = brsh;
            else
                ent->lastbrsh = ent->lastbrsh->next = brsh;
        }
        else
        {
            pair = AllocEntPair();
            pair->next = 0;
            memcpy(pair->key, key, sizeof(key));
            memcpy(pair->val, val, sizeof(val));
            
            if(!ent->npairs)
                ent->pairs = pair;
            else
                lastpair->next = pair;
            
            lastpair = pair;
            ent->npairs++;
        }
    } while(true);
    
    return ent;
}

brushdef_t* ParseBrush()
{
    int i;
    
    const char* ln = "( %d %d %d ) ( %d %d %d ) ( %d %d %d ) %s [ %d %d %d %d ] [ %d %d %d %d ] %d %d %d";
    brushdef_t* brsh;
    planedef_t pl, *ppl;
    vec3_t p1, p2, p3, a, b;
    int ip1[3], ip2[3], ip3[3];
    char texname[17];
    vec3_t s, t;
    int is[3], it[3];
    int sshift, tshift;
    int rot, sscale, tscale;
    
    vec3_t corner;
    
    brsh = (brushdef_t*) malloc(sizeof(brushdef_t));
    
    do
    {
        NextLine();
        if(!strcmp(line, "}"))
            break;
        
        memset(texname, 0, sizeof(texname));
        sscanf(line, ln, &ip1[0], &ip1[1], &ip1[2], &ip2[0], &ip2[1], &ip2[2], &ip3[0], &ip3[1], &ip3[2], texname, &is[0], &is[1], &is[2], &sshift, &it[0], &it[1], &it[2], &tshift, &rot, &sscale, &tscale);
        
        p1[0] = ip1[0]; p1[1] = ip1[1]; p1[2] = ip1[2];
        p2[0] = ip2[0]; p2[1] = ip2[1]; p2[2] = ip2[2];
        p3[0] = ip3[0]; p3[1] = ip3[1]; p3[2] = ip3[2];
        s[0] = is[0]; s[1] = is[1]; s[2] = is[2];
        t[0] = it[0]; t[1] = it[1]; t[2] = it[2];
        
        VectorSubtract(a, p3, p1);
        VectorSubtract(b, p2, p1);
        VectorCross(pl.n, a, b);
        VectorNormalize(pl.n, pl.n);
        pl.d = VectorDot(pl.n, p1);
        memcpy(pl.texname, texname, sizeof(texname));
        VectorCopy(pl.shat, s);
        VectorCopy(pl.that, t);
        pl.sshift = sshift;
        pl.tshift = tshift;
        
        AddPlane(brsh, pl);
    } while(strcmp(line, "}"));
    
    GenBB(brsh);
    for(ppl=brsh->firstpl; ppl; ppl=ppl->next)
    {
        for(i=0, VectorCopy(corner, vec3_origin); i<3; i++)
        {
            if (ppl->n[i] > 0)
                corner[i] = hmaxs[curhull][i];
            else if (ppl->n[i] < 0)
                corner[i] = hmins[curhull][i];
        }
        ppl->d += VectorDot(ppl->n, corner);
    }
    for(i=0; i<3; i++)
    {
        VectorCopy(pl.n, vec3_origin);
        pl.n[i] = 1;
        pl.d = brsh->bbmax[i] + hmaxs[curhull][i];
        AddPlane(brsh, pl);
        
        VectorCopy(pl.n, vec3_origin);
        pl.n[i] = -1;
        pl.d = brsh->bbmin[i] + hmins[curhull][i];
        AddPlane(brsh, pl);
    }
    GenPolys(brsh);
    CutPolys(brsh);
    
    return brsh;
}

void Finish()
{
    int i, j, b;
    
    brushdef_t *br;
    entitydef_t *ent;
    polynode_t *p;
    vnode_t *v;
    char *outname;
    
    if(curhull == 0)
        WriteEnts();
    
    for(ent=firstent; ent; ent=ent->next)
    {
        for(br=ent->firstbrsh; br; br=br->next)
        {
            Optimize(br, ent);
        }
    }
    
#if 0
    for(ent=firstent; ent; ent=ent->next)
    {
        for(br=ent->firstbrsh; br; br=br->next)
        {
            printf("Cut polygons of brush:\n");
            for(i=0, p=br->firstp; p; i++, p=p->next)
            {
                printf("Polygon %d:\n", i+1);
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
    }
#endif
    
    for(ent=firstent, b=0; ent; ent=ent->next)
    {
        if(ent->firstbrsh)
        {
            for(br=ent->firstbrsh; br; br=br->next)
            {
                for(i=0, p=br->firstp; p; i++, p=p->next)
                {
                    WritePoly(*p, hullouts[curhull]);
                }
            }
            b++;
        }
    }
}

void NextLine()
{
    if(line) free(line);
    GetLine(&line, mapfile);
    linenum++;
}

void WriteEnts()
{
    int nmodels;
    entitydef_t *ent;
    entitypair_t *newpair, *pair;
    char* outname;
    FILE* entfile;
    
    for(nmodels=0, ent=firstent; ent; ent=ent->next)
    {
        if(ent->firstbrsh)
        {
            newpair = malloc(sizeof(entitypair_t));
            memset(newpair->key, 0, sizeof(newpair->key));
            memset(newpair->val, 0, sizeof(newpair->val));
            strcpy(newpair->key, "model");
            sprintf(newpair->val, "*%d", nmodels);
            newpair->next = ent->pairs;
            ent->pairs = newpair;
            ent->npairs++;
            nmodels++;
        }
    }
    
    outname = malloc(strlen(filename) + 1);
    memcpy(outname, filename, strlen(filename) + 1);
    outname[strlen(outname) - 3] = 'e';
    outname[strlen(outname) - 2] = 'n';
    outname[strlen(outname) - 1] = 't';
    entfile = fopen(outname, "w");
    
    for(ent=firstent; ent; ent=ent->next)
    {
        fprintf(entfile, "{\n");
        
        for(pair=ent->pairs; pair; pair=pair->next)
            fprintf(entfile, "\"%s\" \"%s\"\n", pair->key, pair->val);
        
        fprintf(entfile, "}\n");
    }
    
    fclose(entfile);
    free(outname);
}

void MemClean()
{
    entitydef_t *curent, *nextent;
    entitypair_t *curpair, *nextpair;
    brushdef_t *curbrush, *nextbrush;
    polynode_t *curp, *nextp;
    planedef_t *curpl, *nextpl;
    
    for(curent=firstent; curent; curent=nextent)
    {
        nextent = curent->next;
        
        for(curpair=curent->pairs; curpair; curpair=nextpair)
        {
            nextpair = curpair->next;
            free(curpair);
        }
        
        for(curbrush=curent->firstbrsh; curbrush; curbrush=nextbrush)
        {
            nextbrush = curbrush->next;
            
            for(curp=curbrush->firstp; curp; curp=nextp)
            {
                nextp = curp->next;
                CullPoly(curp, curbrush);
            }
            for(curpl=curbrush->firstpl; curpl; curpl=nextpl)
            {
                nextpl = curpl->next;
                free(curpl);
            }
            
            free(curbrush);
        }
        
        free(curent);
    }
    
    firstent = lastent = 0;
}

void Error(char* msg)
{
    printf("Processing of map \"%s\" failed with error \"%s\"\n", filename, msg);
    exit(1);
}

entitypair_t* AllocEntPair()
{
    return (entitypair_t*) malloc(sizeof(entitypair_t));
}

entitydef_t* AllocEntity()
{
    return (entitydef_t*) malloc(sizeof(entitydef_t));
}
