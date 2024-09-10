//
//  mapdefs.h
//  Brushstroke
//
//  Created by Henry Dunn on 8/21/24.
//

#ifndef mapdefs_h
#define mapdefs_h

#include "polygon.h"

planedef_t test;

#define MAP_MAX_BOUNDS 4096

#define MAX_KEY     32
#define MAX_VALUE   1024

typedef struct
{
    char key[MAX_KEY+1];
    char val[MAX_VALUE+1];
    struct entitypair_t* next;
} entitypair_t;

typedef struct
{
    struct polynode_t* firstp;
    struct polynode_t* lastp;
    planedef_t* firstpl;
    planedef_t* lastpl;
    int nplanes;
    vec3_t bbmin, bbmax;
    struct brushdef_t* next;
} brushdef_t;

typedef struct
{
    entitypair_t* pairs;
    int npairs;
    brushdef_t* firstbrsh;
    brushdef_t* lastbrsh;
    struct entitydef_t *next;
} entitydef_t;

#endif /* mapdefs_h */
