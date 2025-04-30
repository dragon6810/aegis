#include <csg.h>

#include <math.h>
#include <string.h>

#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

void csg_dupebrush(brush_t* brush, brush_t* newbrush)
{
    brface_t *face;

    brface_t *newface, *lastface;

    assert(brush);
    assert(newbrush);

    memcpy(newbrush, brush, sizeof(brush_t));

    lastface = newbrush->faces = NULL;
    for(face=brush->faces; ; face=face->next)
    {
        if(!face)
            break;

        newface = map_allocbrface();
        memcpy(newface, face, sizeof(brface_t));
        if(newface->poly)
            newface->poly = CopyPoly(newface->poly);

        if(lastface)
            lastface->next = newface;
        else
            newbrush->faces = newface;

        lastface = newface;
    }
}

void csg_brushbounds(brush_t* brush)
{
    int i, j;
    brface_t *face;

    assert(brush);

    if(!brush->faces)
    {
        for(i=0; i<2; i++)
            VectorCopy(brush->bounds[i], vec3_origin);
    }

    for(i=0; i<3; i++)
    {
        brush->bounds[0][i] = (8192<<1);
        brush->bounds[1][i] = -(8192<<1);
    }

    csg_generatefaces(brush);
    for(face=brush->faces; ; face=face->next)
    {
        if(!face)
            break;

        if(!face->poly)
            continue;

        for(i=0; i<face->poly->npoints; i++)
        {
            for(j=0; j<3; j++)
            {
                if(face->poly->points[i][j] < brush->bounds[0][j])
                    brush->bounds[0][j] = face->poly->points[i][j];
                if(face->poly->points[i][j] > brush->bounds[1][j])
                    brush->bounds[1][j] = face->poly->points[i][j];
            }
        }
    }
}

vec3_t hullsizes[MAX_MAP_HULLS][2] = 
{
    { {   0,   0,   0 }, {   0,   0,   0 }, },
    { { -16, -16, -36 }, {  16,  16,  36 }, },
    { { -32, -32, -32 }, {  32,  32,  32 }, },
    { { -16, -16, -18 }, {  16,  16,  18 }, },
};

void csg_expandbrush(brush_t* brush, int hull)
{
    int i, j;
    brface_t *face;

    vec3_t corner;

    assert(brush);
    assert(hull >= 0);
    assert(hull < MAX_MAP_HULLS);

    for(face=brush->faces; ; face=face->next)
    {
        if(!face)
            break;

        VectorCopy(corner, vec3_origin);
        for(i=0; i<3; i++)
        {
            if(face->n[i] < 0)
                corner[i] = hullsizes[hull][0][i];
            if(face->n[i] > 0)
                corner[i] = hullsizes[hull][1][i];
        }
        face->d += VectorDot(corner, face->n);
    }

    if(hull)
    {
        for(i=-1; i<=1; i+=2)
        {
            for(j=0; j<3; j++)
            {
                face = map_allocbrface();
                face->n[j] = i;
                face->d = fabsf(brush->bounds[(i+1)>>1][j]);
                face->next = brush->faces;
                brush->faces = face;
            }
        }
    }
}

void csg_generatefaces(brush_t* brush)
{
    brface_t *face, *_face;

    poly_t *curpoly, *newpoly;

    assert(brush);

#if 0
    printf("-------- brush %d --------\n", nmapbrushes-1);
#endif

    for(face=brush->faces; ; face=face->next)
    {
        if(!face)
            break;

        if(face->poly)
            free(face->poly);

        curpoly = PolyForPlane(face->n, face->d);
        for(_face=brush->faces; ; _face=_face->next)
        {
            if(!_face)
                break;
            if(face == _face)
                continue;

            newpoly = CutPoly(curpoly, _face->n, _face->d, 0);
            free(curpoly);
            curpoly = newpoly;
            if(!curpoly)
                break;
        }

        face->poly = curpoly;

#if 0
        PrintPoly(stdout, face->poly);
        printf("\n");
#endif
    }

#if 0
    printf("\n");
#endif
}

void csg_skinbrushes(void)
{

}

void csg_docsg(void)
{
    int h, i;

    profiler_push("CSG");

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        for(i=0; i<nmapbrushes; i++)
        {
            if(h)
                csg_dupebrush(&maphulls[0][i], &maphulls[h][i]);
            csg_expandbrush(&maphulls[h][i], h);
            csg_generatefaces(&maphulls[h][i]);
        }
    }

    profiler_pop();
}