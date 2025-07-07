#include <bsp/bsp.h>

#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

#include <cli/cli.h>

static void bsp_portalize_r(bsp_plane_t *pl, int h, list_int_t prts)
{
    int i, j;
    bsp_portal_t *curprt;

    list_int_t cpyprts;
    int inewprt;
    bsp_portal_t *newprt;
    vec3_t a, b, n;
    float d;
    poly_t *newpoly;
    list_int_t prtsides[2];
    bsp_portal_t *splitprt;
    poly_t *splitpoly[2];
    int side;
    bsp_leaf_t *curleaf;

    assert(pl);

    LIST_COPY(cpyprts, prts);
    prts = cpyprts;

    if(bsp_nportals[h] >= MAX_MAP_PORTALS)
        cli_error(true, "max map portals reached: max is %d.\n", MAX_MAP_PORTALS);
    inewprt = bsp_nportals[h];
    newprt = &bsp_portals[h][bsp_nportals[h]++];

    newprt->pl = pl;
    newprt->hull = h;
    newprt->leafs[0] = newprt->leafs[1] = NULL;
    newprt->poly = PolyForPlane(pl->n, pl->d);
    newprt->fileprt = -1;
    for(i=0; i<prts.size; i++)
    {
        curprt = &bsp_portals[h][prts.data[i]];
        VectorSubtract(a, curprt->poly->points[1], curprt->poly->points[0]);
        VectorSubtract(b, curprt->poly->points[2], curprt->poly->points[0]);
        VectorCross(n, a, b);
        VectorNormalize(n, n);
        d = VectorDot(curprt->poly->points[0], n);

        newpoly = CutPoly(newprt->poly, n, d, curprt->curside);
        free(newprt->poly);
        newprt->poly = newpoly;

        if(!newprt->poly || newprt->poly->npoints < 3)
            cli_error(true, "bsp_portalize_r: portal clipped out of existence!\n");
    }

    for(i=0; i<2; i++)
    {
        LIST_INITIALIZE(prtsides[i]);
        LIST_RESIZE(prtsides[i], 1);
        prtsides[i].data[0] = bsp_nportals[h]-1;
        for(j=0; j<prts.size; j++)
        {
            curprt = &bsp_portals[h][prts.data[j]];
            side = PolySide(curprt->poly, pl->n, pl->d);
            switch (side)
            {
            case SIDE_BACK:
                if(!i)
                    LIST_PUSH(prtsides[i], prts.data[j]);
               
                break;
            case SIDE_FRONT:
                if(i)
                    LIST_PUSH(prtsides[i], prts.data[j]);
                
                break;
            case SIDE_CROSS:
                if(!i)
                    LIST_PUSH(prtsides[i], prts.data[j]); // since back half stays in place

                if(bsp_nportals[h] >= MAX_MAP_PORTALS)
                    cli_error(true, "max map portals reached: max is %d.\n", MAX_MAP_PORTALS);
                splitprt = &bsp_portals[h][bsp_nportals[h]];
                
                splitpoly[0] = CutPoly(curprt->poly, pl->n, pl->d, SIDE_BACK);
                splitpoly[1] = CutPoly(curprt->poly, pl->n, pl->d, SIDE_FRONT);

                *splitprt = *curprt;
                free(curprt->poly);
                curprt->poly = splitpoly[0];
                splitprt->poly = splitpoly[1];

                if(curprt->pl)
                    LIST_PUSH(curprt->pl->portals, bsp_nportals[h]);
                if(curprt->leafs[0])
                    LIST_PUSH(curprt->leafs[0], bsp_nportals[h]);
                if(curprt->leafs[1])
                    LIST_PUSH(curprt->leafs[1], bsp_nportals[h]);
                LIST_PUSH(prts, bsp_nportals[h]);

                bsp_nportals[h]++;

                break;
            default:
                cli_error(true, "bsp_portalize_r: unexpected portal-plane alignment\n");
                break;
            }
        }
    }

    for(i=0; i<2; i++)
    {
        newprt->curside = i;
        
        if(pl->children[i] > 0)
        {
            bsp_portalize_r(&bsp_planes[h][pl->children[i]], h, prtsides[i]);
            LIST_FREE(prtsides[i]);
        }
        else
        {
            curleaf = bsp_leaves[h][~pl->children[i]];
            newprt->leafs[i] = curleaf;
            LIST_PUSH(curleaf->portals, inewprt);
        }
    }
}

static void bsp_portalize_makeboundportals(void)
{
    const float pad = 8;
    const int windings[6][4] = 
    {
        { 0, 4, 6, 2, }, // 000 100 110 010 -x
        { 1, 3, 7, 5, }, // 001 011 111 101 +x
        { 0, 1, 5, 4, }, // 000 001 101 100 -y
        { 2, 6, 7, 3, }, // 010 110 111 011 +y
        { 0, 2, 3, 1, }, // 000 010 011 001 -z
        { 4, 5, 7, 6, }, // 100 101 111 110 +z
    };

    int h, i, j;

    bsp_plane_t *headnode;
    bsp_portal_t *prt;
    vec3_t corners[8];

    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        headnode = &bsp_planes[h][bsp_models[0].headplane[h]];
        for(i=0; i<8; i++)
        {
            for(j=0; j<3; j++)
            {
                if(i & (1<<j))
                    corners[i][j] = headnode->bounds[1][j] + pad;
                else
                    corners[i][j] = headnode->bounds[0][j] - pad;
            }
        }

        for(i=0; i<6; i++)
        {
            if(bsp_nportals[h] >= MAX_MAP_PORTALS)
                cli_error(true, "max map portals reached: max is %d.\n", MAX_MAP_PORTALS);
            prt = &bsp_portals[h][bsp_nportals[h]++];

            prt->hull = h;
            prt->pl = NULL;
            prt->leafs[0] = prt->leafs[1] = NULL;
            prt->curside = 1;
            prt->poly = AllocPoly(4);
            prt->fileprt = -1;
            for(j=0; j<4; j++)
                VectorCopy(prt->poly->points[j], corners[windings[i][3-j]]); // reversing winding because i forgot
        }
    }
}

void bsp_portalize(void)
{
    int h, i;

    list_int_t prts;

    profiler_push("Portalize World");

    bsp_portalize_makeboundportals();
    for(h=0; h<MAX_MAP_HULLS; h++)
    {
        LIST_INITIALIZE(prts);
        LIST_RESIZE(prts, bsp_nportals[h]);
        for(i=0; i<bsp_nportals[h]; i++)
            prts.data[i] = i;

        bsp_portalize_r(&bsp_planes[h][bsp_models[0].headplane[h]], h, prts);

        LIST_FREE(prts);
    }

    profiler_pop();
}