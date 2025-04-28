#ifndef _map_h
#define _map_h

#include <bspfile.h>
#include <entity.h>

extern int nmapentities;
extern entity_t mapentities[MAX_MAP_ENTITIES];

void map_parsemap(void);

#endif