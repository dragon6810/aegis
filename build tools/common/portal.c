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

portalnode_t* FindPortalInNode(splitplane_t* node, portal_t* p)
{
    portalnode_t* curp;

    for(curp=node->portals; curp; curp=curp->next)
    {
        if(p == curp->p)
            return curp;
    }

    return NULL;
}

void Portalize_r(splitplane_t* curnode)
{
    int i;
    portalnode_t* curprt;
    splitplane_t* node;

    portal_t* p;
    portalnode_t* newp;
    int side;

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
            side = PolyPlaneSide(curprt->p->poly, curnode->n, curnode->d);

            if(side == -1 && i)
                continue;
            if(side == 1 && !i)
                continue;

            if(side == 2)
            {
                p = AllocPortal();
                p->poly = CopyPoly(curprt->p->poly);
                ClipPoly(p->poly, curnode->n, curnode->d, i);
                ClipPoly(curprt->p->poly, curnode->n, curnode->d, !i);
                
                newp = calloc(1, sizeof(portalnode_t));
                newp->p = p;

                for(node=curnode; node && FindPortalInNode(node, curprt->p); node=node->parent)
                    AddPortalToNode(node, p);
            }
            else
                p = curprt->p;

            AddPortalToNode(curnode->children[i], p);

            if(curprt->p->nodes[0] == curnode)
            {
                p->nodes[0] = curnode->children[i];
                p->nodes[1] = curprt->p->nodes[1];
            }
            else if(curprt->p->nodes[1] == curnode)
            {
                p->nodes[0] = curprt->p->nodes[0];
                p->nodes[1] = curnode->children[i];
            }
            else // Should be curnodes portal
            {
                p->nodes[0] = curprt->p->nodes[0];
                p->nodes[1] = curprt->p->nodes[1];
            }
        }

        Portalize_r(curnode->children[i]);
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

void FillWorld_r(splitplane_t* leaf)
{
    surfnode_t* surf;
    portalnode_t* p;

    int i;

    if(leaf == &outsidenode)
        return;

    if(leaf->leaf->marked || leaf->leaf->contents == CONTENTS_SOLID)
        return;
    
    leaf->leaf->marked = true;
    for(surf=leaf->leaf->surfs; surf; surf=surf->next)
        surf->surf->marked = true;

    for(p=leaf->leaf->portals; p; p=p->next)
    {
        if(p->p->nodes[0] == leaf)
            i = 1;
        else
            i = 0;

        FillWorld_r(p->p->nodes[i]);
    }
}

void FillWorld(splitplane_t* head, vec3_t pos)
{
    splitplane_t* leaf;

    leaf = PosToLeaf(pos, head);
    FillWorld_r(leaf);
}

surfnode_t* PruneSurfs(surfnode_t* list)
{
    surfnode_t* cur;
    surfnode_t* next;
    vnode_t* curv, *nextv;

    for(cur=list; cur; cur=next)
    {
        next = cur->next;
        if(cur->surf->marked)
            continue;
        
        if(cur == list)
            list = next;
        
        for(curv=cur->surf->geo.first;; curv=nextv)
        {
            nextv = curv->next;

            free(curv);

            if(nextv == cur->surf->geo.first)
                break;
        }
        free(cur->surf);

        if(cur->last)
            cur->last->next = cur->next;
        if(cur->next)
            cur->next->last = cur->last;

        free(cur);
    }

    return list;
}