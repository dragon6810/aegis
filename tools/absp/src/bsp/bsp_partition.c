#include <bsp/bsp.h>

#include <math.h>
#include <string.h>

#include <std/assert/assert.h>
#include <std/math/math.h>
#include <std/profiler/profiler.h>

#include <cli/cli.h>

void bsp_partition_boundfaces(list_int_t faces, int hull, vec3_t min, vec3_t max)
{
    int i, j, k;

    bsp_face_t *face;

    for(i=0; i<3; i++)
    {
        max[i] = -MAX_MAP_RANGE - 8;
        min[i] =  MAX_MAP_RANGE + 8;
    }

    for(i=0; i<faces.size; i++)
    {
        face = &bsp_faces[hull][faces.data[i]];
        for(j=0; j<face->poly->npoints; j++)
        {
            for(k=0; k<3; k++)
            {
                if(face->poly->points[j][k] < min[k])
                    min[k] = face->poly->points[j][k];
                if(face->poly->points[j][k] > max[k])
                    max[k] = face->poly->points[j][k];
            }
        }
    }
}

void bsp_partition_boundmodel(bsp_model_t* model)
{
    int h, i;

    list_int_t faces;

    assert(model);

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        LIST_INITIALIZE(faces);
        LIST_RESIZE(faces, model->nfaces[h]);

        for(i=0; i<model->nfaces[h]; i++)
            faces.data[i] = model->firstface[h] + i;

        bsp_partition_boundfaces(faces, h, model->bounds[h][0], model->bounds[h][1]);

        LIST_FREE(faces);
    }
}

/* TODO: Currently approximating with monte carlo. make more sophisticated in the future. */
int bsp_partition_spaceratio(vec3_t n, float d, vec3_t bounds[2])
{
    const float epsilon = 0.01;

    int s[3], i;

    vec3_t p;
    float dist;
    int counts[2] = { 0, 0 };

    const int nsamples = 4;

    for(s[0]=0; s[0]<nsamples; s[0]++)
    {
        for(s[1]=0; s[1]<nsamples; s[1]++)
        {
            for(s[2]=0; s[2]<nsamples; s[2]++)
            {
                for(i=0; i<3; i++)
                    p[i] = bounds[0][i] + ((float) s[i] / (float) nsamples) * (bounds[1][i] - bounds[0][i]);
                dist = VectorDot(p, n) - d;

                if(dist < -epsilon)
                    counts[0]++;
                if(dist > epsilon)
                    counts[1]++;
            }
        }
    }

    return abs(counts[1] - counts[0]);
}

int bsp_partition_geometryratio(list_int_t faces, int hull, vec3_t n, float d)
{
    int i;
    bsp_face_t *face;

    int counts[4];
    int side;
    int score;

    for(i=0; i<4; i++)
        counts[i] = 0;

    for(i=0; i<faces.size; i++)
    {
        face = &bsp_faces[hull][faces.data[i]];
        side = PolySide(face->poly, n, d);
        counts[side]++;
    }

    score = abs(counts[SIDE_FRONT] - counts[SIDE_BACK]);
    score = MATH_MIN(0, score - (counts[SIDE_ON] >> 1));
    score += counts[SIDE_CROSS];

    return score;
}

int bsp_partition_chooseplane(list_int_t faces, int hull)
{
    int i;
    bsp_face_t *f;

    vec3_t bounds[2];
    vec3_t n, a, b;
    float d;
    int score;
    int bestscore, bestface;

    bsp_partition_boundfaces(faces, hull, bounds[0], bounds[1]);

    bestface = -1;
    for(i=0; i<faces.size; i++)
    {
        f = &bsp_faces[hull][faces.data[i]];
        if(f->poly->npoints < 3 || f->plane >= 0)
            continue;

        VectorSubtract(a, f->poly->points[1], f->poly->points[0]);
        VectorSubtract(b, f->poly->points[2], f->poly->points[0]);
        VectorCross(n, a, b);
        VectorNormalize(n, n);
        d = VectorDot(n, f->poly->points[0]);
        score = bsp_partition_spaceratio(n, d, bounds) + bsp_partition_geometryratio(faces, hull, n, d);

        if(bestface < 0 || score < bestscore)
        {
            bestface = i;
            bestscore = score;
        }
    }

    return bestface;
}

/* back stays in place, returns index to front */
int bsp_partition_splitface(int iface, int h, vec3_t n, float d)
{
    bsp_face_t *oldface, *newface;
    poly_t *oldpoly;
    int inewface;

    oldface = &bsp_faces[h][iface];
    if(PolySide(oldface->poly, n, d) != SIDE_CROSS)
        return iface;
    
    if(bsp_nfaces[h] >= MAX_MAP_FACES)
        cli_error(true, "map exceeds max faces, max is %d per hull\n", MAX_MAP_FACES);

    inewface = bsp_nfaces[h]++;
    newface = &bsp_faces[h][inewface];
    memcpy(newface, oldface, sizeof(bsp_face_t));
    newface->poly = CutPoly(newface->poly, n, d, SIDE_FRONT);
    if(newface->plane >= 0)
        LIST_PUSH(bsp_planes[h][newface->plane].faces, inewface);

    oldpoly = oldface->poly;
    oldface->poly = CutPoly(oldpoly, n, d, SIDE_BACK);
    free(oldpoly);

    return inewface;
}

void bsp_partition_seperatebyplane(list_int_t faces, int hull, vec3_t n, float d, list_int_t* back, list_int_t* front)
{
    int i;
    bsp_face_t *face;
    
    list_int_t backlist, frontlist;
    int side;
    int inewface;

    assert(back);
    assert(front);

    LIST_INITIALIZE(backlist);
    LIST_INITIALIZE(frontlist);
    for(i=0; i<faces.size; i++)
    {
        face = &bsp_faces[hull][faces.data[i]];
        side = PolySide(face->poly, n, d);
        switch(side)
        {
        case SIDE_ON:
            continue;
        case SIDE_BACK:
            LIST_PUSH(backlist, faces.data[i]);
            continue;
        case SIDE_FRONT:
            LIST_PUSH(frontlist, faces.data[i]);
            continue;
        }

        // split
        inewface = bsp_partition_splitface(faces.data[i], hull, n, d);
        LIST_PUSH(backlist, faces.data[i]);
        LIST_PUSH(frontlist, inewface);
    }

    memcpy(back, &backlist, sizeof(list_int_t));
    memcpy(front, &frontlist, sizeof(list_int_t));
}

int bsp_partition_addleaf(list_int_t faces, int hull, int side)
{
    int ileaf;
    bsp_leaf_t* leaf;

    if(bsp_nleaves[hull] >= MAX_MAP_LEAFS)
        cli_error(true, "map exceeds max leaves, max is %d per hull\n", MAX_MAP_LEAFS);
    
    ileaf = bsp_nleaves[hull]++;
    leaf = bsp_leaves[hull][ileaf] = malloc(sizeof(bsp_leaf_t) + sizeof(int) * faces.size);
    leaf->fileclipleaf = -1;

    leaf->contents = side ? LEAF_CONTENT_EMPTY : LEAF_CONTENT_SOLID;
    leaf->hull = hull;
    LIST_INITIALIZE(leaf->portals);
    leaf->nfaces = faces.size;
    memcpy(leaf->faces, faces.data, sizeof(int) * faces.size);

    // printf("leaf n faces: %d\n", leaf->nfaces);

    return ileaf;
}

int bsp_parition_split_r(list_int_t faces, int hull, int side)
{
    int i;
    bsp_face_t *face;

    int isplitface, iplane;
    bsp_face_t *splitface;
    bsp_plane_t *plane;
    list_int_t offplane, facelists[2];
    vec3_t n, a, b;
    float d;

    isplitface = bsp_partition_chooseplane(faces, hull);
    if(isplitface < 0)
        return ~bsp_partition_addleaf(faces, hull, side);

    if(bsp_nplanes[hull] >= MAX_MAP_PLANES)
        cli_error(true, "map exceeds max planes: max is %d per hull\n", MAX_MAP_PLANES);
    iplane = bsp_nplanes[hull]++;
    plane = &bsp_planes[hull][iplane];
    LIST_INITIALIZE(plane->portals);
    splitface = &bsp_faces[hull][faces.data[isplitface]];
    splitface->plane = iplane;

    if(splitface->poly->npoints < 3)
        cli_error(true, "invalid geometry: face has <3 points\n");
    
    VectorSubtract(a, splitface->poly->points[1], splitface->poly->points[0]);
    VectorSubtract(b, splitface->poly->points[2], splitface->poly->points[0]);
    VectorCross(n, a, b);
    VectorNormalize(n, n);
    d = VectorDot(n, splitface->poly->points[0]);

    bsp_partition_boundfaces(faces, hull, plane->bounds[0], plane->bounds[1]);
    LIST_INITIALIZE(plane->faces);
    plane->hull = hull;
    VectorCopy(plane->n, n);
    plane->d = d;

    LIST_INITIALIZE(offplane);
    for(i=0; i<faces.size; i++)
    {
        face = &bsp_faces[hull][faces.data[i]];

        if(PolySide(face->poly, plane->n, plane->d) != SIDE_ON)
        {
            LIST_PUSH(offplane, faces.data[i]);
            continue;
        }

        LIST_PUSH(plane->faces, faces.data[i]);
        face->plane = iplane;
    }

    bsp_partition_seperatebyplane(offplane, hull, plane->n, plane->d, &facelists[0], &facelists[1]);
    for(i=0; i<2; i++)
    {
        LIST_INSERTLIST(facelists[i], plane->faces, facelists[i].size);
        plane->children[i] = bsp_parition_split_r(facelists[i], hull, i);
        LIST_FREE(facelists[i]);
    }

    LIST_FREE(offplane);

    return iplane;
}

void bsp_partition_processmodel(bsp_model_t* model)
{
    int h, i;

    list_int_t faces;

    assert(model);

    bsp_partition_boundmodel(model);
    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        LIST_INITIALIZE(faces);
        LIST_RESIZE(faces, model->nfaces[h]);
        for(i=0; i<model->nfaces[h]; i++)
            faces.data[i] = model->firstface[h] + i;

        model->headplane[h] = bsp_parition_split_r(faces, h, 1);

        LIST_FREE(faces);
    }
}

void bsp_partition(void)
{
    int i;

    profiler_push("Partition World");

    for(i=0; i<bsp_nmodels; i++)
        bsp_partition_processmodel(&bsp_models[i]);

    if(cli_verbose)
    {
        printf("-+------+--------+-------+--------+-\n");
        printf(" | hull | planes | faces | leaves | \n");
        for(i=0; i<MAX_MAP_HULLS; i++)
        {
            printf("-+------+--------+-------+--------+-\n");
            printf(" | %4d | %6d | %5d | %6d | \n", i, bsp_nplanes[i], bsp_nfaces[i], bsp_nleaves[i]);
        }
        printf("-+------+--------+-------+--------+-\n");
    }

    profiler_pop();
}