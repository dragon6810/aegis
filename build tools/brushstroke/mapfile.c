//
//  mapfile.c
//  Brushstroke
//
//  Created by Henry Dunn on 8/22/24.
//

#include "mapfile.h"

int nhulls;
int linenum = 0;

void ParseMap(char* name)
{
    filename = name;
    
    printf("Parsing map \"%s\"...\n", name);
    
    mapfile = fopen(name, "r");
    if(!mapfile)
        Error("Bad map file path");
    
    NextLine();
    while(ParseEntry());
}

boolean ParseEntry()
{
    char c;
    void* entry;
    
    if(strcmp(line, "{"))
        return false;
    
    c = fgetc(mapfile);
    fseek(mapfile, -1, SEEK_CUR);
    if(c == '"')
        ParseEntity();
    
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
    memset(ent, 0, sizeof(ent));
    
    do
    {
        NextLine();
        
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
                ent->firstbrsh->next = ent->firstbrsh = ent->lastbrsh = brsh;
            else
                ent->lastbrsh = ent->lastbrsh->next = brsh;
        }
        else
        {
            pair = AllocEntPair();
            memcpy(pair->key, key, sizeof(key));
            memcpy(pair->val, val, sizeof(val));
            
            if(!ent->npairs)
                ent->pairs = pair;
            else
                lastpair->next = pair;
            
            lastpair = pair;
            ent->npairs++;
        }
    } while(strcmp(line, "}"));
    
    return ent;
}

brushdef_t* ParseBrush()
{
    const char* ln = "( %d %d %d ) ( %d %d %d ) ( %d %d %d ) %s [ %d %d %d %d ] [ %d %d %d %d ] %d %d %d";
    brushdef_t* brsh;
    planedef_t pl;
    vec3_t p1, p2, p3, a, b;
    int ip1[3], ip2[3], ip3[3];
    char texname[17];
    vec3_t s, t;
    int is[3], it[3];
    int sshift, tshift;
    int rot, sscale, tscale;
    
    brsh = (brushdef_t*) malloc(sizeof(brushdef_t));
    
    do
    {
        NextLine();
        
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
        
        AddPlane(brsh, pl);
    } while(strcmp(line, "}"));
    
    GenPolys(brsh);
    CutPolys(brsh);
    
    return brsh;
}

void NextLine()
{
    if(line) free(line);
    GetLine(&line, mapfile);
    linenum++;
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
