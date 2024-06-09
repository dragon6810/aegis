#include "BSPMap.h"

#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "binaryloader.h"
#include "loadtexture.h"
#include "AssetManager.h"

void BSPMap::Load(const char* filename)
{
	loadBytes(filename, (char**) &mhdr);
	
	printf("Loading Map %s (version %d).\n", filename, mhdr->nVersion);

	bsptextureheader_t* texhdr = (bsptextureheader_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset);
	gltextures = (int*)malloc(texhdr->nMipTextures * sizeof(int));
	for (int i = 0; i < texhdr->nMipTextures; i++)
	{
		int** texdata = (int**) malloc(sizeof(int*) * BSP_MIPLEVELS);
		int width, height;

		int texoffset = *((int*)(texhdr + i + 1));
		miptex_t* miptex = (miptex_t*)((char*)texhdr + texoffset);

		loadmiptex((char*)miptex, texdata, &width, &height);

		if (miptex->offsets[0] == 0 || miptex->offsets[1] == 0 || miptex->offsets[2] == 0 || miptex->offsets[3] == 0)
		{
			gltextures[i] = AssetManager::getInst().getTexture(miptex->name, "wad");
		}
		else
		{
			gltextures[i] = AssetManager::getInst().setTexture(miptex->name, filename);

			glBindTexture(GL_TEXTURE_2D, gltextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width >> 0, height >> 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[0]);
			glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, width >> 1, height >> 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[1]);
			glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, width >> 2, height >> 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[2]);
			glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, width >> 3, height >> 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[3]);
		}
	}

	int numfaces = mhdr->lump[BSP_LUMP_FACES].nLength / sizeof(bspface_t);
	facebounds = (vec2_t*) malloc(numfaces * sizeof(vec2_t));
	facecenters = (vec3_t*) malloc(numfaces * sizeof(vec3_t));
	for (int i = 0; i < numfaces; i++)
	{
		bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + i;
		facebounds[i] = FaceBounds(i, &facecenters[i]);
	}

	lightmaptextures = (int*) malloc(numfaces * sizeof(int));
	for (int i = 0; i < numfaces; i++)
	{
		bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + i;
		bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset) + face->iTextureInfo;
		color24_t* lightmap = (color24_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LIGHTING].nOffset + face->nLightmapOffset);

		if (face->nLightmapOffset < 0)
		{
			int texdata = 0xFFFFFFFF;

			char name[14];
			sprintf(name, "lightmap%d", i);
			lightmaptextures[i] = AssetManager::getInst().setTexture(name, "map");
			
			glBindTexture(GL_TEXTURE_2D, lightmaptextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata);

			continue;
		}

		int luxelsx = (int)(facebounds[i].x * 2.0) / BSP_LIGHTMAP_LUXELLEN + 1;
		int luxelsy = (int)(facebounds[i].y * 2.0) / BSP_LIGHTMAP_LUXELLEN + 1;

		int* texdata = (int*)malloc(sizeof(int) * luxelsx * luxelsy);
		memset(texdata, 0xFF000000, sizeof(int) * luxelsx * luxelsy);
		for (int j = 0; j < luxelsx * luxelsy; j++)
		{
			color24_t col = *lightmap;
			texdata[j] |= (int)col.r <<  0;
			texdata[j] |= (int)col.g <<  8;
			texdata[j] |= (int)col.b << 16;
			lightmap++;
		}

		char name[14];
		sprintf(name, "lightmap%d", i);
		lightmaptextures[i] = AssetManager::getInst().setTexture(name, "map");
		
		glBindTexture(GL_TEXTURE_2D, lightmaptextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, luxelsx, luxelsy, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);

		free(texdata);
	}
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
	int firstchild = side < 0;

	RenderNode(node->iChildren[firstchild]);

	for (int i = node->firstFace; i < node->firstFace + node->nFaces; i++)
		RenderFace(i);

	RenderNode(node->iChildren[!firstchild]);
}

void BSPMap::RenderLeaf(short leafnum)
{
	bspleaf_t* leaf = (bspleaf_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LEAVES].nOffset) + leafnum;
	uint16_t* marksurfaces = (uint16_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_MARKSURFACES].nOffset);

	for (int i = leaf->iFirstMarkSurface; i < leaf->nMarkSurfaces + leaf->iFirstMarkSurface; i++)
		RenderFace(marksurfaces[i]);
}

void BSPMap::RenderFace(uint16_t f)
{
	bspplane_t* planes = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset);
	bspface_t* faces = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset);
	vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
	bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset);
	int* texoffsets = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset);

	bspface_t* face = faces + f;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gltextures[texinfo[face->iTextureInfo].iMiptex]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lightmaptextures[f]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	miptex_t* miptex = (miptex_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset + texoffsets[texinfo[face->iTextureInfo].iMiptex]);
	vec3_t normal = planes[face->iPlane].vNormal;
	if (face->nPlaneSide != 0)
	{
		normal.x = -normal.x;
		normal.y = -normal.y;
		normal.z = -normal.z;
	}

	glCullFace(GL_FRONT);

	glBegin(GL_POLYGON);
	for (int j = face->iFirstEdge; j < face->iFirstEdge + face->nEdges; j++)
	{
		vec3_t pos;
		if (surfedges[j] >= 0)
			pos = vertices[edges[surfedges[j]].iVertex[0]];
		else
			pos = vertices[edges[-surfedges[j]].iVertex[1]];

		float s = pos.x * texinfo[face->iTextureInfo].vS.x + pos.y * texinfo[face->iTextureInfo].vS.y + pos.z * texinfo[face->iTextureInfo].vS.z;
		s += texinfo[face->iTextureInfo].fSShift;
		s /= miptex->width;
		float t = pos.x * texinfo[face->iTextureInfo].vT.x + pos.y * texinfo[face->iTextureInfo].vT.y + pos.z * texinfo[face->iTextureInfo].vT.z;
		t += texinfo[face->iTextureInfo].fTShift;
		t /= miptex->height;

		float sl = texinfo[face->iTextureInfo].vS.x * texinfo[face->iTextureInfo].vS.x;
		sl += texinfo[face->iTextureInfo].vS.y * texinfo[face->iTextureInfo].vS.y;
		sl += texinfo[face->iTextureInfo].vS.z * texinfo[face->iTextureInfo].vS.z;
		sl = sqrtf(sl);
		float tl = texinfo[face->iTextureInfo].vT.x * texinfo[face->iTextureInfo].vT.x;
		tl += texinfo[face->iTextureInfo].vT.y * texinfo[face->iTextureInfo].vT.y;
		tl += texinfo[face->iTextureInfo].vT.z * texinfo[face->iTextureInfo].vT.z;
		tl = sqrtf(tl);

		vec2_t lightmapCoords{};

		lightmapCoords.x = (pos.x - facecenters[f].x) * texinfo[face->iTextureInfo].vS.x / sl;
		lightmapCoords.x += (pos.y - facecenters[f].y) * texinfo[face->iTextureInfo].vS.y / sl;
		lightmapCoords.x += (pos.z - facecenters[f].z) * texinfo[face->iTextureInfo].vS.z / sl;

		lightmapCoords.y = (pos.x - facecenters[f].x) * texinfo[face->iTextureInfo].vT.x / tl;
		lightmapCoords.y += (pos.y - facecenters[f].y) * texinfo[face->iTextureInfo].vT.y / tl;
		lightmapCoords.y += (pos.z - facecenters[f].z) * texinfo[face->iTextureInfo].vT.z / tl;

		int luxelsx = (int)(facebounds[f].x * 2.0) / BSP_LIGHTMAP_LUXELLEN + 1;
		int luxelsy = (int)(facebounds[f].y * 2.0) / BSP_LIGHTMAP_LUXELLEN + 1;

		lightmapCoords.x = (lightmapCoords.x / BSP_LIGHTMAP_LUXELLEN) / luxelsx + 0.5f;
		lightmapCoords.y = (lightmapCoords.y / BSP_LIGHTMAP_LUXELLEN) / luxelsy + 0.5f;

		glMultiTexCoord2f(GL_TEXTURE0, s, t);
		glMultiTexCoord2f(GL_TEXTURE1, lightmapCoords.x, lightmapCoords.y);
		glVertex3f(pos.x, pos.y, pos.z);
	}
	glEnd();

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
}

vec2_t BSPMap::FaceBounds(uint16_t f, vec3_t* facecenter)
{
	bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + f;
	vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
	bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset);

	*facecenter = { 0.0, 0.0, 0.0 };
	for (int i = face->iFirstEdge; i < face->iFirstEdge + face->nEdges; i++)
	{
		vec3_t pos;
		if (surfedges[i] >= 0)
			pos = vertices[edges[surfedges[i]].iVertex[0]];
		else
			pos = vertices[edges[-surfedges[i]].iVertex[1]];

		facecenter->x += pos.x;
		facecenter->y += pos.y;
		facecenter->z += pos.z;
	}

	facecenter->x /= face->nEdges;
	facecenter->y /= face->nEdges;
	facecenter->z /= face->nEdges;

	vec3_t up = texinfo[face->iTextureInfo].vT;
	vec3_t right = texinfo[face->iTextureInfo].vS;
	float upl = sqrtf(up.x * up.x + up.y * up.y + up.z * up.z);
	float rightl = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
	up.x /= upl; up.y /= upl; up.z /= upl;
	right.x /= rightl; right.y /= rightl; right.z /= rightl;

	float maxup = 0.0, maxright = 0.0;
	for (int i = face->iFirstEdge; i < face->iFirstEdge + face->nEdges; i++)
	{
		vec3_t pos;
		if (surfedges[i] > 0)
			pos = vertices[edges[surfedges[i]].iVertex[0]];
		else
			pos = vertices[edges[-surfedges[i]].iVertex[1]];

		pos.x -= facecenter->x;
		pos.y -= facecenter->y;
		pos.z -= facecenter->z;

		float updot = fabs(pos.x * up.x + pos.y * up.y + pos.z * up.z);
		float rightdot = fabs(pos.x * right.x + pos.y * right.y + pos.z * right.z);

		if (updot > maxup)
			maxup = updot;
		if (rightdot > maxright)
			maxright = rightdot;
	}

	return { maxright, maxup };
}

vec2_t BSPMap::FaceCoordinates(uint16_t f, vec3_t p)
{
	bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + f;
	vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
	bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset);

	vec3_t facecenter = { 0.0, 0.0, 0.0 };
	for (int i = face->iFirstEdge; i < face->iFirstEdge + face->nEdges; i++)
	{
		if (surfedges[i] > 0)
		{
			facecenter.x += vertices[edges[surfedges[i]].iVertex[0]].x;
			facecenter.y += vertices[edges[surfedges[i]].iVertex[0]].y;
			facecenter.z += vertices[edges[surfedges[i]].iVertex[0]].z;
		}
		else
		{
			facecenter.x += vertices[edges[-surfedges[i]].iVertex[1]].x;
			facecenter.y += vertices[edges[-surfedges[i]].iVertex[1]].y;
			facecenter.z += vertices[edges[-surfedges[i]].iVertex[1]].z;
		}
	}

	facecenter.x /= face->nEdges;
	facecenter.y /= face->nEdges;
	facecenter.z /= face->nEdges;

	vec3_t up = texinfo[face->iTextureInfo].vT;
	vec3_t right = texinfo[face->iTextureInfo].vS;
	float upl = sqrtf(up.x * up.x + up.y * up.y + up.z * up.z);
	float rightl = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
	up.x /= upl; up.y /= upl; up.z /= upl;
	right.x /= rightl; right.y /= rightl; right.z /= rightl;

	p.x -= facecenter.x;
	p.y -= facecenter.y;
	p.z -= facecenter.z;

	return { up.x * p.x + up.y * p.y + up.z * p.z, right.x * p.x + right.y * p.y + right.z * p.z };
}

BSPMap::~BSPMap()
{
	free(gltextures);
	free(facebounds);
	free(facecenters);
	free(lightmaptextures);
}