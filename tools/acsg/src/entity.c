#include <entity.h>

#include <stdlib.h>
#include <string.h>

#include <std/assert/assert.h>

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