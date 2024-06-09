#include "BSPMap.h"

#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>

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

		if(miptex->offsets[0] == 0 || miptex->offsets[1] == 0 || miptex->offsets[2] == 0 || miptex->offsets[3] == 0)
			gltextures[i] = AssetManager::getInst().getTextureIndex(miptex->name, "wad");
		else
			gltextures[i] = AssetManager::getInst().getTextureIndex(miptex->name, filename);

		if (gltextures[i] < 0)
		{
			gltextures[i] = -gltextures[i] + 1;
			glGenTextures(1, (GLuint*)&gltextures[i]);
			glBindTexture(GL_TEXTURE_2D, gltextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width >> 0, height >> 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[0]);
			glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, width >> 1, height >> 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[1]);
			glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, width >> 2, height >> 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[2]);
			glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, width >> 3, height >> 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[3]);
		}
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
	bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset);
	int* texoffsets = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

	for (int i = leaf->iFirstMarkSurface; i < leaf->nMarkSurfaces + leaf->iFirstMarkSurface; i++)
	{
		glBindTexture(GL_TEXTURE_2D, gltextures[texinfo[faces[i].iTextureInfo].iMiptex]);
		miptex_t* miptex = (miptex_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset + texoffsets[texinfo[faces[i].iTextureInfo].iMiptex]);
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

			float s = pos.x * texinfo[faces[i].iTextureInfo].vS.x + pos.y * texinfo[faces[i].iTextureInfo].vS.y + pos.z * texinfo[faces[i].iTextureInfo].vS.z;
			s += texinfo[faces[i].iTextureInfo].fSShift;
			s /= miptex->width;
			float t = pos.x * texinfo[faces[i].iTextureInfo].vT.x + pos.y * texinfo[faces[i].iTextureInfo].vT.y + pos.z * texinfo[faces[i].iTextureInfo].vT.z;
			t += texinfo[faces[i].iTextureInfo].fTShift;
			t /= miptex->height;
			glTexCoord2f(s, t);
			glColor4f(light, light, light, 1.0);
			glVertex3f(pos.x, pos.y, pos.z);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}

BSPMap::~BSPMap()
{
	free(gltextures);
}