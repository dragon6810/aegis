#include <entity.h>

#include <stdlib.h>
#include <string.h>

#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

#include <globals.h>

int nmapentities = 0;
entity_t mapentities[MAX_MAP_ENTITIES] = {};

epair_t* entity_allocepair(void)
{
    return calloc(1, sizeof(epair_t));
}

epair_t* entity_findbykey(entity_t* ent, const char* key)
{
    epair_t* pair;

    assert(ent);
    assert(key);

    for(pair=ent->pairs; pair->next!=ent->pairs; pair=pair->next)
    {
        if(!pair)
            break;

        if(strcmp(pair->key, key))
            continue;

        return pair;
    }

    return NULL;
}

void entity_writeents(void)
{
    int i;

    FILE *ptr;
    epair_t *pair;

    profiler_push("Write Entities");

    ptr = fopen(entfilepath, "w");
    if(!ptr)
        return;

    for(i=0; i<nmapentities; i++)
    {
        fprintf(ptr, "{\n");

        for(pair=mapentities[i].pairs; ; pair=pair->next)
        {
            if(!pair)
                break;

            fprintf(ptr, "\"%s\" \"%s\"\n", pair->key, pair->val);
        }

        fprintf(ptr, "}\n");
    }

    profiler_pop();
}