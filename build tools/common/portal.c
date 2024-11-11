#include "portal.h"

splitplane_t outsidenode;

portal_t* AllocPortal()
{
    return (portal_t*) calloc(1, sizeof(portal_t));
}

portalnode_t* AllocPortalNode()
{
    return (portalnode_t*)calloc(1, sizeof(portalnode_t));
}

void AddPortalToNode(splitplane_t* node, portal_t* p)
{
    portalnode_t* curprt;

    portalnode_t* pn;

    pn = AllocPortalNode();
    pn->p = p;

    if (!node->portals)
    {
        node->portals = pn;
        return;
    }

    for (curprt = node->portals; curprt->next; curprt = curprt->next) {}

    curprt->next = pn;
    pn->last = curprt;
}

void Portalize_r(splitplane_t* curnode)
{
    int i;
    portalnode_t* curprt;

    portal_t* p;

    if (curnode->leaf)
    {
        curnode->leaf->portals = curnode->portals;
        return;
    }

    p = AllocPortal();
    p->poly = AllocPoly();
    HungryPoly(p->poly, curnode->n, curnode->d);

    for (curprt = curnode->portals; curprt; curprt = curprt->next)
        ClipPoly(p->poly, curprt->p->n, curprt->p->d, 1);

    p->nodes[0] = curnode->children[0];
    p->nodes[1] = curnode->children[1];

    AddPortalToNode(curnode, p);

    for (i = 0; i < 2; i++)
    {
        for (curprt = curnode->portals; curprt; curprt = curprt->next)
        {
            p = AllocPortal();
            p->poly = CopyPoly(curprt->p->poly);
            ClipPoly(p->poly, curnode->n, curnode->d, i);
            if (!p->poly->first)
            {
                free(p->poly);
                free(p);
                continue;
            }

            AddPortalToNode(curnode->children[i], p);
        }
    }
}

void HeadnodePortals(splitplane_t* head)
{
    int i, j, n;

    vec3_t min, max;
    vec3_t mins[3], maxs[3];

    portal_t* portals[6];
    portal_t* p, **p1, **p2;

    SurfListBB(head->childsurfs[0], &mins[0], &maxs[0]);
    SurfListBB(head->surfs, &mins[1], &maxs[1]);
    SurfListBB(head->childsurfs[1], &mins[2], &maxs[2]);

    for (i = 0; i < 3; i++)
    {
        min[i] = mins[0][i] < mins[1][i] && mins[0][i] < mins[2][i] ? mins[0][i] : mins[1][i] < mins[2][i] ? mins[1][i] : mins[2][i]; // what the fuck
        max[i] = maxs[0][i] > maxs[1][i] && maxs[0][i] > maxs[2][i] ? maxs[0][i] : maxs[1][i] > maxs[2][i] ? maxs[1][i] : maxs[2][i];

        min[i] -= 24;
        max[i] += 24;
    }

    outsidenode.portals = 0;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 2; j++)
        {
            n = j * 3 + i;

            p = AllocPortal();
            portals[n] = p;

            if (j)
            {
                p->n[i] = -1;
                p->d = -max[i];
            }
            else
            {
                p->n[i] = 1;
                p->d = min[i];
            }

            p->poly = AllocPoly();
            HungryPoly(p->poly, p->n, p->d);

            p->nodes[0] = &outsidenode;
            p->nodes[1] = head;

            AddPortalToNode(&outsidenode, p);
            AddPortalToNode(head, p);
        }
    }

    for (p1 = portals; p1 < portals + 6; p1++)
    {
        for (p2 = portals; p2 < portals + 6; p2++)
        {
            if (p1 == p2)
                continue;

            ClipPoly((*p1)->poly, (*p2)->n, (*p2)->d, 1);
        }
    }
}

void Portalize(splitplane_t* head)
{
	HeadnodePortals(head);
    Portalize_r(head);
}

void FillWorld(splitplane_t* head, surfnode_t* surfs, vec3_t pos)
{
    leaf_t* leaf;

    leaf = PosToLeaf(pos, head);
}