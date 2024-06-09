#include "BSPMap.h"

#include <GL/glew.h>

#include <stdio.h>

#include "binaryloader.h"

void BSPMap::Load(const char* filename)
{
	loadBytes(filename, (char**) &mhdr);
	
	printf("Loading Map %s (version %d).\n", filename, mhdr->nVersion);
}

void BSPMap::SetCameraPosition(vec3_t pos)
{
	camerapos = pos;
}

void BSPMap::Draw()
{
	RenderNode(1);
}

void BSPMap::RenderNode(short nodenum)
{
	if (nodenum <= 0)
	{
		RenderLeaf(~nodenum);
		return;
	}

 	bspnode_t* node = (bspnode_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_NODES].nOffset) + nodenum;
	bspplane_t* plane = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset) + node->iPlane;

	float side = plane->vNormal.x * camerapos.x + plane->vNormal.y * camerapos.y + plane->vNormal.z * camerapos.z - plane->fDist;
	
	if (side < 0.0)
	{
		RenderNode(node->iChildren[1]);
		RenderNode(node->iChildren[0]);
	}
	else
	{
		RenderNode(node->iChildren[0]);
		RenderNode(node->iChildren[1]);
	}
}

void BSPMap::RenderLeaf(short leafnum)
{
	bspplane_t* planes = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset);
	bspleaf_t* leaf = (bspleaf_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LEAVES].nOffset) + leafnum;
	bspface_t* faces = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset);
	uint16_t* marksurfaces = (uint16_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_MARKSURFACES].nOffset);
	vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);

	for (int i = leaf->iFirstMarkSurface; i < leaf->nMarkSurfaces + leaf->iFirstMarkSurface; i++)
	{
		vec3_t normal = planes[faces[i].iPlane].vNormal;
		if (faces[i].nPlaneSide != 0)
		{
			normal.x = -normal.x;
			normal.y = -normal.y;
			normal.z = -normal.z;
		}
		float light = normal.z + 0.5;

		glCullFace(GL_FRONT);

		glBegin(GL_POLYGON);
		for (int j = faces[i].iFirstEdge; j < faces[i].iFirstEdge + faces[i].nEdges; j++)
		{
			vec3_t pos;
			if (surfedges[j] >= 0)
				pos = vertices[edges[surfedges[j]].iVertex[0]];
			else
				pos = vertices[edges[-surfedges[j]].iVertex[1]];

			glColor4f(light, light, light, 1.0);
			glVertex3f(pos.x, pos.y, pos.z);
		}
		glEnd();
	}
}