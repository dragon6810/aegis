#include <bsp/bsp.h>

#include <stdlib.h>

#include <std/profiler/profiler.h>

#include <cli/cli.h>

bsp_entity_t* bsp_loadents_allocent(int npairs)
{
    bsp_entity_t *ent;

    ent = calloc(1, sizeof(bsp_entity_t) + sizeof(bsp_epair_t) * npairs);
    ent->npairs = npairs;

    return ent;
}

void bsp_loadents(void)
{
    int i;

    FILE *ptr;
    unsigned long int before;
    int npairs;
    bsp_entity_t *ent;
    char key[ENTITY_MAX_KEY], val[ENTITY_MAX_VAL];

    ptr = fopen(cli_entfile, "r");
    if(!ptr)
        return;

    profiler_push("Load Entities");

    while(fgetc(ptr) == '{')
    {
        fseek(ptr, 1, SEEK_CUR);

        npairs = 0;
        before = ftell(ptr);
        while(fscanf(ptr, "\"%31[^\"]\" \"%1023[^\"]\"\n", key, val))
            npairs++;
        fseek(ptr, before, SEEK_SET);

        if(bsp_nentities >= MAX_MAP_ENTITIES)
            cli_error(true, "map exceeds max entities, max is %d\n", MAX_MAP_ENTITIES);

        ent = bsp_entities[bsp_nentities++] = bsp_loadents_allocent(npairs);
        for(i=0; i<npairs; i++)
            fscanf(ptr, "\"%31[^\"]\" \"%1023[^\"]\"\n", ent->pairs[i].key, ent->pairs[i].val);
        
        if(fgetc(ptr) != '}')
            cli_error(true, "error parsing entity, expected '}'\n");
        fseek(ptr, 1, SEEK_CUR);
    }

    if(cli_verbose)
        printf("entity count: %d\n", bsp_nentities);

    profiler_pop();
}