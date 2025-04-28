#ifndef _ENTITY_H
#define _ENTITY_H

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
} entity_t;

epair_t* entity_allocepair(void);
epair_t* entity_findbykry(entity_t* ent, const char* key);

#endif