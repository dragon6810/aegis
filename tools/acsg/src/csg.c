#include <csg.h>

#include <math.h>
#include <stdbool.h>
#include <string.h>

#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

#include <entity.h>
#include <globals.h>

vec3_t hullsizes[MAX_MAP_HULLS][2] = 
{
    { {   0,   0,   0 }, {   0,   0,   0 }, },
    { { -16, -16, -36 }, {  16,  16,  36 }, },
    { { -32, -32, -32 }, {  32,  32,  32 }, },
    { { -16, -16, -18 }, {  16,  16,  18 }, },
};

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

void csg_expandbrush(brush_t* brush, int hull)
{
    int i, j;
    brface_t *face, *lastface;

    vec3_t corner;

    assert(brush);
    assert(hull >= 0);
    assert(hull < MAX_MAP_HULLS);

    csg_brushbounds(brush);

    lastface = NULL;
    for(face=brush->faces; ; face=face->next)
    {
        if(!face)
            break;
        lastface = face;

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

    if(hull && lastface)
    {
        for(i=-1; i<=1; i+=2)
        {
            for(j=0; j<3; j++)
            {
                face = map_allocbrface();
                face->n[j] = i;
                face->d = fabsf(brush->bounds[(i+1)/2][j])+fabsf(hullsizes[hull][(i+1)/2][j]);
                strcpy(face->miptex, "NULL");
                lastface->next = face;
                lastface = face;
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

bool csg_boundsintersect(brush_t* a, brush_t* b)
{
    int i;

    assert(a);
    assert(b);

    for (i=0; i<3; i++)
        if (a->bounds[0][i] > b->bounds[1][i] || a->bounds[1][i] < b->bounds[0][i])
            return false;

	return true;
}

void csg_cleanbrush(brush_t* brush)
{
    brface_t *face, *lastface, *nextface;

    for(face=brush->faces,lastface=NULL; ; face=nextface)
    {
        if(!face)
            break;
        
        if(face->poly)
        {
            nextface=face->next;
            lastface = face;
            continue;
        }
        
        if(lastface)
            lastface->next = face->next;
        else
            brush->faces = face->next;
        nextface = face->next;

        free(face);
    }
}

bool csg_polyinsidebrush(poly_t* poly, brush_t* srcbrush, brush_t* brush)
{
    const float epsilon = 0.01;

    int i;

    brface_t *face;
    vec3_t a, b, n;

    assert(poly);
    assert(brush);

    for(face=brush->faces; face; face=face->next)
    {
        for(i=0; i<poly->npoints; i++)
        {
            if(VectorDot(poly->points[i], face->n) - face->d > epsilon)
                return false;
        }
    }

    for(face=brush->faces; face; face=face->next)
    {
        if(!PolyOnPlane(poly, face->n, face->d))
            continue;

        if(poly->npoints < 3)
            continue;

        VectorSubtract(a, poly->points[1], poly->points[0]);
        VectorSubtract(b, poly->points[2], poly->points[0]);
        VectorCross(n, a, b);
        if(VectorDot(n, face->n) < 0)
            continue;

        // Poly is coincident & aligned to plane, choose one arbitrarily

        return srcbrush < brush;
    }

    return true;
}

void csg_prunefaces(brush_t* a, brush_t* b)
{
    brface_t *face;

    assert(a);
    assert(b);

    if(!csg_boundsintersect(a, b))
        return;

    for(face=a->faces; face; face=face->next)
    {
        if(!face->poly || !csg_polyinsidebrush(face->poly, a, b))
            continue;

        face->poly = NULL;
    }
}

void csg_skinabyb(brush_t* a, brush_t* b)
{
    brface_t *facea, *faceb, *lastface;

    poly_t *fpoly, *bpoly;
    brface_t *fface, *bface;
    brface_t *newfaces;

    assert(a);
    assert(b);

    if(a == b)
        return;

    if(!csg_boundsintersect(a, b))
        return;

    csg_cleanbrush(a);

    newfaces = NULL;
    for(faceb=b->faces; faceb; faceb=faceb->next)
    {
        lastface = NULL;
        for(facea=a->faces; facea; facea=facea->next)
        {   
            if(!facea->poly)
            {
                lastface = facea;
                continue;
            }

            if(PolyOnPlane(facea->poly, faceb->n, faceb->d))
            {
                lastface = facea;
                continue;
            }

            bpoly = CutPoly(facea->poly, faceb->n, faceb->d, 0);
            fpoly = CutPoly(facea->poly, faceb->n, faceb->d, 1);

            if(!bpoly && !fpoly)
            {
                /* this should never happen */
                lastface = facea;
                continue;
            }

            if(bpoly && !fpoly)
            {
                free(bpoly);
                lastface = facea;
                continue;
            }

            if(!bpoly && fpoly)
            {
                free(fpoly);
                lastface = facea;
                continue;
            }

            bface = map_allocbrface();
            memcpy(bface, facea, sizeof(brface_t));
            bface->poly = bpoly;
            bface->next = newfaces;
            newfaces = bface;

            fface = map_allocbrface();
            memcpy(fface, facea, sizeof(brface_t));
            fface->poly = fpoly;
            fface->next = newfaces;
            newfaces = fface;

            facea->poly = NULL;
            lastface = facea;
        }
    }

    if(lastface)
        lastface->next = newfaces;
}

void csg_skinmodel(int firstbrush, int nbrushes)
{
    int h, i, j;

    for(h=0; h<MAX_MAP_HULLS; h++)
        for(i=firstbrush; i<firstbrush+nbrushes-1; i++)
            csg_cleanbrush(&maphulls[h][i]);

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        for(i=firstbrush; i<firstbrush+nbrushes; i++)
        {
            for(j=firstbrush; j<firstbrush+nbrushes; j++)
            {
                if(i == j)
                    continue;

                csg_skinabyb(&maphulls[h][i], &maphulls[h][j]);
            }
        }
    }

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        for(i=firstbrush; i<firstbrush+nbrushes; i++)
        {
            for(j=firstbrush; j<firstbrush+nbrushes; j++)
            {
                if(i == j)
                    continue;

                csg_prunefaces(&maphulls[h][i], &maphulls[h][j]);
            }
        }
    }

    for(h=0; h<MAX_MAP_HULLS; h++)
        for(i=firstbrush; i<firstbrush+nbrushes-1; i++)
            csg_cleanbrush(&maphulls[h][i]);
}

void csg_skinbrushes(void)
{
    int i;

    for(i=0; i<nmapentities; i++)
    {
        if(!mapentities[i].nbrushes)
            continue;

        csg_skinmodel(mapentities[i].firstbrush, mapentities[i].nbrushes);
    }
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
            csg_generatefaces(&maphulls[h][i]);
            csg_expandbrush(&maphulls[h][i], h);
            csg_generatefaces(&maphulls[h][i]);
            csg_brushbounds(&maphulls[h][i]);
        }
    }

    csg_skinbrushes();

    profiler_pop();
}

void csg_writefaces(void)
{
    int h, i, j, b, k, n;
    brush_t *brush;
    brface_t *face;
    vec3_t *o;

    FILE *ptr;

    profiler_push("Write Faces");

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        ptr = fopen(outfilepaths[h], "w");
        if(!ptr)
        {
            fprintf(stderr, "couldn't open file \"%s\" for writing.\n", outfilepaths[h]);
            abort();
        }

        for(i=j=0; i<nmapentities; i++)
        {
            if(!mapentities[i].nbrushes)
                continue;

            o = &mapentities[i].origin;
            fprintf(ptr, "*%d ( %f %f %f )\n", j, (*o)[0], (*o)[1], (*o)[2]);
            for(b=mapentities[i].firstbrush; b<mapentities[i].firstbrush+mapentities[i].nbrushes; b++)
            {
                brush = &maphulls[h][b];
                for(face=brush->faces,n=0; ; face=face->next,n++)
                {
                    if(!face)
                        break;

                    if(!face->poly)
                        continue;

                    for(k=0; k<face->poly->npoints; k++)
                        fprintf(ptr, "( %f %f %f ) ", 
                            face->poly->points[k][0], face->poly->points[k][1], face->poly->points[k][2]);

                    fprintf(ptr, "%s [ %f %f %f %f ] [ %f %f %f %f ] %f %f\n",
                        face->miptex,
                        face->texvec[0][0], face->texvec[0][1], face->texvec[0][2], face->shift[0],
                        face->texvec[1][0], face->texvec[1][1], face->texvec[1][2], face->shift[1],
                        face->scale[0], face->scale[1]);
                }
            }

            j++;
        }

        fclose(ptr);
    }

    profiler_pop();
}