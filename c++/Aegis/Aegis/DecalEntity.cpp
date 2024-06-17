#include "DecalEntity.h"

#include "AssetManager.h"

DecalEntity::DecalEntity(BSPMap& map) : BaseEntity(map)
{

}

void DecalEntity::SetTexture(char* texname)
{
	memcpy(texture, texname, strlen(texname));

	waddirentry_t* entry = (waddirentry_t*)((char*)decalswad->whdr + decalswad->whdr->diroffset);

	for (int i = 0; i < decalswad->whdr->numdirs; i++, entry++)
	{
		miptex_t* texture = (miptex_t*)((char*)decalswad->whdr + entry->entryoffset);

		if (strcmp(texname, entry->name) != 0)
			continue;

		size = texture->width >> 1;
		if ((texture->height >> 1) > size)
			size = texture->height >> 1;

		texindex = AssetManager::getInst().getTexture(texname, "wad");

		break;
	}
}

void DecalEntity::SetWad(Wad& wad)
{
	this->decalswad = &wad;
}

void DecalEntity::Init()
{
	RecursiveFindFaces(0);
	/*
	int numfaces = map->mhdr->lump[BSP_LUMP_FACES].nLength / sizeof(bspface_t);

	vec3_t* vertices = (vec3_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
	
	
	std::vector<int> possiblefaces;
	map->BoxIntersect({ position.x - 8, position.y - 8, position.z - 8 }, { position.x + 8, position.y + 8, position.z + 8 }, 0, possiblefaces);
	for (int f = 0; f < possiblefaces.size(); f++)
	{
		bspface_t* face = (bspface_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_FACES].nOffset) + possiblefaces[f];

		std::vector<vec3_t> poly;
		for (int e = face->iFirstEdge; e < face->iFirstEdge + face->nEdges; e++)
		{
			vec3_t pos;
			if (surfedges[e] >= 0)
				pos = vertices[edges[surfedges[e]].iVertex[0]];
			else
				pos = vertices[edges[-surfedges[e]].iVertex[1]];

			poly.push_back(pos);
		}

		poly = BoxFace({ position.x - 32, position.y - 32, position.z - 32 }, { position.x + 32, position.y + 32, position.z + 32 }, poly);

		if (poly.size() > 0)
			faces.push_back(poly);
	}
	*/
	
	/*
	for (int f = 0; f < numfaces; f++)
	{
		bspface_t* face = (bspface_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_FACES].nOffset) + f;

		std::vector<vec3_t> poly;
		for (int e = face->iFirstEdge; e < face->iFirstEdge + face->nEdges; e++)
		{
			vec3_t pos;
			if (surfedges[e] >= 0)
				pos = vertices[edges[surfedges[e]].iVertex[0]];
			else
				pos = vertices[edges[-surfedges[e]].iVertex[1]];

			poly.push_back(pos);
		}

		poly = BoxFace({ position.x - 8, position.y - 8, position.z - 8 }, { position.x + 8, position.y + 8, position.z + 8 }, poly);

		if (poly.size() > 0)
			faces.push_back(poly);
	}
	*/
}

void DecalEntity::RecursiveFindFaces(int nodenum)
{
	if (nodenum < 0)
		return;

	bspnode_t* node = (bspnode_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_NODES].nOffset) + nodenum;
	bspplane_t* plane = (bspplane_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_PLANES].nOffset) + node->iPlane;
	float dist = position.x * plane->vNormal.x + position.y * plane->vNormal.y + position.z * plane->vNormal.z - plane->fDist;

	if (dist > size)
		RecursiveFindFaces(node->iChildren[0]);
	else if (dist < -size)
		RecursiveFindFaces(node->iChildren[1]);
	else
	{
		if (dist < DECAL_DISTANCE && dist > -DECAL_DISTANCE)
			AddFaces(nodenum);

		RecursiveFindFaces(node->iChildren[0]);
		RecursiveFindFaces(node->iChildren[1]);
	}
}

void DecalEntity::AddFaces(int nodenum)
{
	if (nodenum < 0)
		return;

	vec3_t* vertices = (vec3_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);

	bspnode_t* node = (bspnode_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_NODES].nOffset) + nodenum;
	bspface_t* face = (bspface_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_FACES].nOffset) + node->firstFace;

	for (int i = 0; i < node->nFaces; i++, face++)
	{
		std::vector<vec3_t> poly;
		for (int e = face->iFirstEdge; e < face->iFirstEdge + face->nEdges; e++)
		{
			vec3_t pos;
			if (surfedges[e] >= 0)
				pos = vertices[edges[surfedges[e]].iVertex[0]];
			else
				pos = vertices[edges[-surfedges[e]].iVertex[1]];

			poly.push_back(pos);
		}

		poly = BoxFace({ position.x - size, position.y - size, position.z - size }, { position.x + size, position.y + size, position.z + size }, poly);

		if (poly.size() > 0)
			faces.push_back(poly);
	}
}

void DecalEntity::Render()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0f, -1.5f);
	for (int f = 0; f < faces.size(); f++)
	{
		glBegin(GL_POLYGON);
		for (int v = 0; v < faces[f].size(); v++)
		{
			vec3_t vert = faces[f][v];
			glColor3f(0, 1, 0);
			glVertex3f(vert.x, vert.y, vert.z);
		}
		glEnd();
	}

	glDisable(GL_POLYGON_OFFSET_FILL);
	glColor3f(1, 1, 1);
}