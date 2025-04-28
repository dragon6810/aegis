#include <vector.h>

#include <stdlib.h>

void VectorCopy(vec3_t dest, vec3_t vec)
{
    int i;

    for(i=0; i<3; i++)
        dest[i] = vec[i];
}

poly_t* AllocPoly(int npoints)
{
    return calloc(1, sizeof(poly_t) + sizeof(vec3_t) * npoints);
}