#ifndef _ENTITY_H
#define _ENTITY_H

#include <map.h>

#define ENTITY_MAX_KEY 32
#define ENTITY_MAX_VAL 1024

typedef struct epair_s
{
    struct epair_s *next;
    char key[ENTITY_MAX_KEY], val[ENTITY_MAX_VAL];
} epair_t;

typedef struct entity_s
{
    epair_t *pairs;
    int nbrushes;
    int firstbrush;
    vec3_t origin;
} entity_t;

extern int nmapentities;
extern entity_t mapentities[MAX_MAP_ENTITIES];

epair_t* entity_allocepair(void);
epair_t* entity_findbykey(entity_t* ent, const char* key);
void entity_writeents(void);

#endif