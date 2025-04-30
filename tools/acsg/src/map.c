#include <map.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <parselib/parselib.h>

#include <globals.h>
#include <entity.h>
#include <csg.h>

int nmapbrushes = 0;
brush_t mapbrushes[MAX_MAP_BRUSHES] = {};

tokenstate_t map_parse;

brface_t* map_allocbrface(void)
{
    return calloc(1, sizeof(brface_t));
}

brush_t* map_allocbrush(void)
{
    return calloc(1, sizeof(brush_t));
}

void map_nextpair(void)
{
    epair_t *pair;

    if(strlen(map_parse.token) < 2 || strlen(map_parse.token) >= ENTITY_MAX_KEY - 2)
    {
        fprintf(stderr, "error: entity key too long, max is %d.\n", ENTITY_MAX_KEY);
        abort();
    }

    pair = entity_allocepair();
    memcpy(pair->key, map_parse.token+1, strlen(map_parse.token)-2);
    pair->key[strlen(map_parse.token)-2] = 0;

    if(!parselib_nexttoken(&map_parse))
    {
        fprintf(stderr, "error: expected entity pair in \"%s\".\n", sourcefilepath);
        abort();
    }

    if(strlen(map_parse.token) < 2 || strlen(map_parse.token) >= ENTITY_MAX_VAL - 2)
    {
        fprintf(stderr, "error: entity val too long, max is %d.\n", ENTITY_MAX_VAL);
        abort();
    }

    memcpy(pair->val, map_parse.token+1, strlen(map_parse.token)-2);
    pair->val[strlen(map_parse.token)-2] = 0;

    pair->next = mapentities[nmapentities].pairs;
    mapentities[nmapentities].pairs = pair;
}

void map_nextbrush(void)
{
    int i, j;

    brush_t *brush;
    brface_t *curface;
    vec3_t plpoints[3], a, b;

    brush = &mapbrushes[nmapbrushes++];

    while(true)
    {
        if(!parselib_nexttoken(&map_parse))
        {
            fprintf(stderr, "error: brush entry doesn't have an end\n");
            abort();
        }

        if(!strcmp(map_parse.token, "}"))
            break;

        for(i=0; i<3; i++)
        {
            if(i)
                parselib_nexttoken(&map_parse);

            if(strcmp(map_parse.token, "("))
            {
                fprintf(stderr, "error: brush face entry doesn't have three plane points\n");
                abort();
            }

            for(j=0; j<3; j++)
            {
                if(!parselib_nexttoken(&map_parse))
                {
                    fprintf(stderr, "error: brush face plane point isn't complete vector\n");
                    abort();
                }

                plpoints[i][j] = atoi(map_parse.token);
            }

            if(!parselib_nexttoken(&map_parse) || strcmp(map_parse.token, ")"))
            {
                fprintf(stderr, "error: brush face entry doesn't have three plane points\n");
                abort();
            }
        }

        curface = map_allocbrface();

        if(!parselib_nexttoken(&map_parse) || strlen(map_parse.token) >= 16)
        {
            fprintf(stderr, "error: brush face entry doesn't have miptex name\n");
            abort();
        }

        strcpy(curface->miptex, map_parse.token);

        for(i=0; i<2; i++)
        {
            if(!parselib_nexttoken(&map_parse) || strcmp(map_parse.token, "["))
            {
                fprintf(stderr, "error: brush face entry doesn't have u vector\n");
                abort();
            }

            for(j=0; j<3; j++)
            {
                if(!parselib_nexttoken(&map_parse))
                {
                    fprintf(stderr, "error: brush face entry doesn't have texture vector\n");
                    abort();
                }

                curface->texvec[i][j] = atoi(map_parse.token);
            }

            if(!parselib_nexttoken(&map_parse))
            {
                fprintf(stderr, "error: brush face entry doesn't have texture vector\n");
                abort();
            }
            curface->shift[i] = atoi(map_parse.token);

            if(!parselib_nexttoken(&map_parse) || strcmp(map_parse.token, "]"))
            {
                fprintf(stderr, "error: brush face entry doesn't have u vector\n");
                abort();
            }
        }

        if(!parselib_nexttoken(&map_parse))
        {
            fprintf(stderr, "error: brush face entry doesn't have rotation\n");
            abort();
        }

        for(i=0; i<2; i++)
        {
            if(!parselib_nexttoken(&map_parse))
            {
                fprintf(stderr, "error: brush face entry doesn't have tex scale\n");
                abort();
            }
            curface->scale[i] = atof(map_parse.token);
        }

        VectorSubtract(a, plpoints[1], plpoints[0]);
        VectorSubtract(b, plpoints[2], plpoints[0]);
        VectorCross(curface->n, b, a);
        VectorNormalize(curface->n, curface->n);
        curface->d = VectorDot(curface->n, plpoints[0]);

        curface->next = brush->faces;
        brush->faces = curface;
    }

    csg_generatefaces(brush);

    brush->nextbrush = mapentities[nmapentities].brushes;
    mapentities[nmapentities].brushes = brush;
}

bool map_parsenext(void)
{
    if(!parselib_nexttoken(&map_parse))
        return false;

    if(strcmp(map_parse.token, "{"))
    {
        fprintf(stderr, "error: expected entity entry in \"%s\".\n", sourcefilepath);
        abort();
    }

    if(nmapentities >= MAX_MAP_ENTITIES)
    {
        fprintf(stderr, "error: too many entities, max is %d.\n", MAX_MAP_ENTITIES);
        abort();
    }

    while(1)
    {
        if(!parselib_nexttoken(&map_parse))
            return false;
        if(!strcmp(map_parse.token, "}"))
            break;
        if(!strcmp(map_parse.token, "{"))
            map_nextbrush();
        else
            map_nextpair();
    }

    nmapentities++;

    return true;
}

void map_parsemap(void)
{
    FILE *ptr;
    char *buf;
    unsigned long int buflen;

    ptr = fopen(sourcefilepath, "r");
    if(!ptr)
    {
        fprintf(stderr, "error: mapfile does not exist \"%s\".\n", sourcefilepath);
        abort();
    }
    fseek(ptr, 0, SEEK_END);
    buflen = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);
    buf = malloc(buflen + 1);
    fread(buf, 1, buflen, ptr);
    buf[buflen] = 0;
    fclose(ptr);
    parselib_start(buf, &map_parse);
    free(buf);

    while(map_parsenext());

    parselib_stop(&map_parse);
}