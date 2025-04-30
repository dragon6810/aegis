#ifndef _map_h
#define _map_h

#include <bspfile.h>
#include <vector.h>

typedef struct brface_s
{
    struct brface_s *next;
    vec3_t n;
    float d;
    poly_t* poly;
    vec3_t texvec[2];
    float shift[2], scale[2];
    char miptex[16];
} brface_t;

typedef struct brush_s
{
    brface_t *faces;
    vec3_t bounds[2];
} brush_t;

extern int nmapbrushes;
extern brush_t maphulls[MAX_MAP_HULLS][MAX_MAP_BRUSHES];

brface_t* map_allocbrface(void);
void map_parsemap(void);

#endif