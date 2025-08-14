#include "Portals.h"

void Portalize_r(int nodenum, const std::vector<int>& prts)
{
    int i, j;

    node_t *node;
    int inewprt;
    portal_t *newprt, portal;
    plane_t *pl, fpl;
    Mathlib::planeside_e side;
    std::vector<int> sides[2];
    int leafnum;
    leaf_t *leaf;

    if(nodenum < 0)
    {
        leafnum = ~nodenum;
        leaf = &leaves[leafnum];
        leaf->portals.resize(prts.size());
        for(i=0; i<prts.size(); i++)
        {
            leaf->portals[i] = prts[i];
            portals[prts[i]].leafnums[portals[prts[i]].curside] = leafnum;
        }

        return;
    }

    node = &nodes[nodenum];
    node->portals.push_back(portals.size());
    inewprt = portals.size();
    portals.push_back({});
    newprt = &portals.back();

    newprt->planenum = node->planenum;
    newprt->nodenum = nodenum;
    newprt->leafnums[0] = newprt->leafnums[1] = -1;

    newprt->poly = Mathlib::FromPlane(planes[node->planenum].n, planes[node->planenum].d);
    for(i=0; i<prts.size(); i++)
    {
        fpl = planes[portals[prts[i]].planenum];
        if(portals[prts[i]].flip)
        {
            fpl.n = -fpl.n;
            fpl.d = -fpl.d;
        }
        
        UTILS_ASSERT(portals[prts[i]].curside == 0 || portals[prts[i]].curside == 1);

        if(portals[prts[i]].curside)
            newprt->poly = Mathlib::ClipPoly(newprt->poly, fpl.n, fpl.d, Mathlib::SIDE_FRONT);
        else
            newprt->poly = Mathlib::ClipPoly(newprt->poly, fpl.n, fpl.d, Mathlib::SIDE_BACK);
        UTILS_ASSERT(newprt->poly.size());
    }

    pl = &planes[newprt->planenum];
    for(i=0; i<prts.size(); i++)
    {
        UTILS_ASSERT(newprt->planenum != portals[prts[i]].planenum);

        side = Mathlib::PolySide(portals[prts[i]].poly, pl->n, pl->d);
        UTILS_ASSERT(side != Mathlib::SIDE_ON);
        if(side == Mathlib::SIDE_BACK)
        {
            sides[0].push_back(prts[i]);
            continue;
        }
        if(side == Mathlib::SIDE_FRONT)
        {
            sides[1].push_back(prts[i]);
            continue;
        }

        if(portals[prts[i]].nodenum >= 0)
            nodes[portals[prts[i]].nodenum].portals.push_back(portals.size());
        for(j=0; j<2; j++)
        {
            if(portals[prts[i]].leafnums[j] < 0)
                continue;
            leaves[portals[prts[i]].leafnums[j]].portals.push_back(portals.size());
        }
        sides[0].push_back(prts[i]);
        sides[1].push_back(portals.size());

        portal = portals[prts[i]];
        portals.push_back(portal);
        portals[prts[i]].poly = Mathlib::ClipPoly(portal.poly, pl->n, pl->d, Mathlib::SIDE_BACK);
        portals.back().poly = Mathlib::ClipPoly(portal.poly, pl->n, pl->d, Mathlib::SIDE_FRONT);
        
        newprt = &portals[inewprt];
    }

    for(i=0; i<2; i++)
    {
        for(j=0; j<node->portals.size(); j++)
        {
            // dont flip bportals curside!
            if(portals[node->portals[j]].planenum == node->planenum)
                portals[node->portals[j]].curside = i;
            sides[i].push_back(node->portals[j]);
        }
        Portalize_r(node->children[i], sides[i]);
    }
}

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
            d = nodes[node].bb[!j][i] * (j * 2 - 1) - pad;

            bportals.push_back(portals.size());
            portals.push_back({});
            prt = &portals.back();

            prt->planenum = FindPlane(n, d);
            if(prt->planenum < 0)
            {
                prt->planenum = ~prt->planenum;
                prt->flip = true;
            }
            prt->nodenum = node;
            prt->leafnums[0] = prt->leafnums[1] = -1;
            prt->poly = Mathlib::FromPlane(n, d);
            prt->curside = 1;
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
    std::vector<int> prts;

    UTILS_ASSERT(mdl);

    printf("---- Portalize ----\n");

    startt = TIMEMS;

    for(h=0; h<Bsplib::n_hulls; h++)
    {
        nodes[mdl->headnodes[h]].portals = BoundingPortals(mdl->headnodes[h]);
        prts = nodes[mdl->headnodes[h]].portals;
        Portalize_r(mdl->headnodes[h], prts);
    }

    endt = TIMEMS;
    printf("Portalize done in %llums.\n", endt - startt);
}