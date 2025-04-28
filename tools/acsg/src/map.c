#include <map.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <parselib/parselib.h>

#include <globals.h>

int nmapentities = 0;
entity_t mapentities[MAX_MAP_ENTITIES] = {};

tokenstate_t map_parse;

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

    printf("pair: \"%s\" \"%s\".\n", pair->key, pair->val);
}

void map_nextbrush(void)
{

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