#include "Portals.h"

std::vector<int> BoundingPortals(int node)
{
    const float pad = 16.0;

    int i, j;

    Eigen::Vector3f n;
    float d;
    std::vector<int> bportals;
    int iprt;
    portal_t *prt;
    plane_t *pl;

    bportals.reserve(6);
    for(i=0; i<3; i++)
    {
        for(j=0; j<2; j++)
        {
            n = Eigen::Vector3f::Zero();
            n[i] = j * 2 - 1;
            d = nodes[node].bb[!j][i] * (j * 2 - 1);

            bportals.push_back(portals.size());
            portals.push_back({});
            prt = &portals.back();

            prt->planenum = FindPlane(n, d);
            if(prt->planenum < 0)
            {
                prt->planenum = ~prt->planenum;
                prt->flip = true;
            }
            prt->leafnums[0] = prt->leafnums[1] = -1;
            prt->poly = Mathlib::FromPlane(n, d);
        }
    }

    for(i=0; i<bportals.size(); i++)
    {
        for(j=0; j<bportals.size(); j++)
        {
            if(i == j)
                continue;

            pl = &planes[portals[bportals[j]].planenum];
            if(portals[bportals[j]].flip)
                Mathlib::ClipPoly(portals[bportals[i]].poly, pl->n, pl->d, Mathlib::SIDE_BACK);
            else
                Mathlib::ClipPoly(portals[bportals[i]].poly, pl->n, pl->d, Mathlib::SIDE_FRONT);
        }
    }

    return bportals;
}

void Portalize(model_t* mdl)
{
    int h;

    uint64_t startt, endt;

    UTILS_ASSERT(mdl);

    printf("---- Portalize ----\n");

    startt = TIMEMS;

    for(h=0; h<Bsplib::n_hulls; h++)
    {
        nodes[mdl->headnodes[h]].portals = BoundingPortals(mdl->headnodes[h]);
    }

    endt = TIMEMS;
    printf("Portalize done in %llums.\n", endt - startt);
}