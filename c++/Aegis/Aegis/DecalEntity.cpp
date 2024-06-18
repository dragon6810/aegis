#include "DecalEntity.h"

#include "AssetManager.h"

#include <iostream>

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

		texwidth = texture->width;
		texheight = texture->height;

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
		bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_TEXINFO].nOffset) + face->iTextureInfo;

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

		bspplane_t* plane = (bspplane_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_PLANES].nOffset) + face->iPlane;
		vec3_t normal = plane->vNormal;
		if (face->nPlaneSide != 0)
		{
			normal.x = -normal.x;
			normal.y = -normal.y;
			normal.z = -normal.z;
		}

		if (poly.size() > 0)
		{
			faces.push_back(poly);

			vec3_t SAxis = texinfo->vS;
			vec3_t TAxis = texinfo->vT;

			float slen = sqrtf(SAxis.x * SAxis.x + SAxis.y * SAxis.y + SAxis.z * SAxis.z);
			float tlen = sqrtf(TAxis.x * TAxis.x + TAxis.y * TAxis.y + TAxis.z * TAxis.z);

			SAxis.x /= slen;
			SAxis.y /= slen;
			SAxis.z /= slen;

			TAxis.x /= tlen;
			TAxis.y /= tlen;
			TAxis.z /= tlen;

			SAxis.x *= (float) 1 / texwidth;
			SAxis.y *= (float) 1 / texwidth;
			SAxis.z *= (float) 1 / texwidth;

			TAxis.x *= (float) 1 / texheight;
			TAxis.y *= (float) 1 / texheight;
			TAxis.z *= (float) 1 / texheight;

			dx = position.x * SAxis.x + position.y * SAxis.y + position.z * SAxis.z;
			dy = position.x * TAxis.x + position.y * TAxis.y + position.z * TAxis.z;
			
			std::vector<vec2_t> uvs;
			std::vector<vec2_t> lightcoords;
			for (int v = 0; v < poly.size(); v++)
			{
				float s = poly[v].x * SAxis.x + poly[v].y * SAxis.y + poly[v].z * SAxis.z - dx + 0.5;
				float t = poly[v].x * TAxis.x + poly[v].y * TAxis.y + poly[v].z * TAxis.z - dy + 0.5;

				uvs.push_back({ s, t });

				lightcoords.push_back(map->GetLightmapCoords(i + node->firstFace, poly[v]));
			}

			texcoords.push_back(uvs);
			lightmapcoords.push_back(lightcoords);
		}
	}
}

//OPTIMIZE: Attach a piece of the decal to every face its on so it can be rendered with its face instead of after everything else
void DecalEntity::Render()
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -0.1f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texindex);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	for (int f = 0; f < faces.size(); f++)
	{
		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, map->lightmaptextures[f]);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glBegin(GL_POLYGON);
		for (int v = 0; v < faces[f].size(); v++)
		{
			vec3_t vert = faces[f][v];
			glMultiTexCoord2f(GL_TEXTURE0, texcoords[f][v].x, texcoords[f][v].y);
			glMultiTexCoord2f(GL_TEXTURE1, lightmapcoords[f][v].x, lightmapcoords[f][v].y);
			glVertex3f(vert.x, vert.y, vert.z);
		}
		glEnd();
	}

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_POLYGON_OFFSET_FILL);
}