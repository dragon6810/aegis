#ifndef _PORTAL_H
#define _PORTAL_H

#include "mapdefs.h"
#include "polygon.h"

typedef struct portal_t portal_t;
typedef struct portalnode_t portalnode_t;

struct portal_t
{
	vec3_t n;
	float d;
	splitplane_t* nodes[2];
	polynode_t* poly;
};

struct portalnode_t
{
	portal_t* p;
	portalnode_t* last;
	portalnode_t* next;
};

extern splitplane_t outsidenode;

void AddPortalToNode(splitplane_t* node, portal_t* p);

void Portalize(splitplane_t* head);
void FillWorld(splitplane_t* head, vec3_t pos);

#endif // !_PORTAL_H