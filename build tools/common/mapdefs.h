//
//  mapdefs.h
//  Brushstroke
//
//  Created by Henry Dunn on 8/21/24.
//

#ifndef mapdefs_h
#define mapdefs_h

#include "polygon.h"

#define MAP_MAX_BOUNDS 4096

#define MAX_KEY     32
#define MAX_VALUE   1024

typedef struct brushdef_t brushdef_t;

typedef struct
{
    char key[MAX_KEY+1];
    char val[MAX_VALUE+1];
    struct entitypair_t* next, *last;
} entitypair_t;

struct brushdef_t
{
    struct polynode_t* firstp;
    struct polynode_t* lastp;
    planedef_t* firstpl;
    planedef_t* lastpl;
    int nplanes;
    vec3_t bbmin, bbmax;
    brushdef_t* next;
};

typedef struct
{
    entitypair_t* pairs;
    int npairs;
    brushdef_t* firstbrsh;
    brushdef_t* lastbrsh;
    struct entitydef_t *next;
} entitydef_t;

#endif /* mapdefs_h */
