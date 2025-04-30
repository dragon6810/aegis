#include <csg.h>

#include <std/assert/assert.h>

void csg_generatefaces(brush_t* brush)
{
    brface_t *face, *_face;

    poly_t *curpoly, *newpoly;

    assert(brush);

    printf("-------- brush %d --------\n", nmapbrushes-1);

    for(face=brush->faces; ; face=face->next)
    {
        if(!face)
            break;

        curpoly = PolyForPlane(face->n, face->d);
        for(_face=brush->faces; ; _face=_face->next)
        {
            if(!_face)
                break;
            if(face == _face)
                continue;

            newpoly = CutPoly(curpoly, _face->n, _face->d, 0);
            if(!newpoly)
            {
                fprintf(stderr, "error: invalid brush has plane which gets clipped out of existence\n");
                abort();
            }

            free(curpoly);
            curpoly = newpoly;
        }

        face->poly = curpoly;

        PrintPoly(stdout, face->poly);
        printf("\n");
    }

    printf("\n");
}