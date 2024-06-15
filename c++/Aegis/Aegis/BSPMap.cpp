#include "BSPMap.h"

#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string.h>
#include <math.h>

#include "binaryloader.h"
#include "loadtexture.h"
#include "AssetManager.h"

#include "RotatingEntity.h"
#include "WallEntity.h"

void BSPMap::Load(const char* filename)
{
	loadBytes(filename, (char**) &mhdr);
	
	printf("Loading Map %s (version %d).\n", filename, mhdr->nVersion);

	bsptextureheader_t* texhdr = (bsptextureheader_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset);
	gltextures = (int*)malloc(texhdr->nMipTextures * sizeof(int));
	for (int i = 0; i < texhdr->nMipTextures; i++)
	{
		int texoffset = *((int*)(texhdr + i + 1));
		miptex_t* miptex = (miptex_t*)((char*)texhdr + texoffset);

		if (miptex->offsets[0] == 0 || miptex->offsets[1] == 0 || miptex->offsets[2] == 0 || miptex->offsets[3] == 0)
		{
			gltextures[i] = AssetManager::getInst().getTexture(miptex->name, "wad");
		}
		else
		{
			gltextures[i] = AssetManager::getInst().setTexture(miptex->name, filename);

			int** texdata = (int**)malloc(sizeof(int*) * BSP_MIPLEVELS);
			int width, height;

			loadmiptex((char*)miptex, texdata, &width, &height);

			glBindTexture(GL_TEXTURE_2D, gltextures[i]);

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, BSP_MIPLEVELS - 1);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width >> 0, height >> 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[0]);
			glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, width >> 1, height >> 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[1]);
			glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, width >> 2, height >> 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[2]);
			glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, width >> 3, height >> 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata[3]);

			for (int m = 0; m < BSP_MIPLEVELS; m++)
				free(texdata[m]);

			free(texdata);
		}
	}

	int numfaces = mhdr->lump[BSP_LUMP_FACES].nLength / sizeof(bspface_t);
	lightmaptextures = (int*) malloc(numfaces * sizeof(int));
	
	for (int i = 0; i < numfaces; i++)
	{
		bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + i;
		bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset) + face->iTextureInfo;
		color24_t* lightmap = (color24_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LIGHTING].nOffset + face->nLightmapOffset);
		vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
		bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
		int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
		
		maxtex[i] = { -999999.9, -999999.9 };
		mintex[i] = {  999999.9,  999999.9 };

		//maxtex[i] = { 0.0, 0.0 };
		//mintex[i] = { 0.0, 0.0 };

		for (int e = face->iFirstEdge; e < face->iFirstEdge + face->nEdges; e++)
		{
			vec3_t pos;
			if (surfedges[e] >= 0)
				pos = vertices[edges[surfedges[e]].iVertex[0]];
			else
				pos = vertices[edges[-surfedges[e]].iVertex[1]];

			float s = floor(pos.x * texinfo->vS.x + pos.y * texinfo->vS.y + pos.z * texinfo->vS.z + texinfo->fSShift);
			float t = floor(pos.x * texinfo->vT.x + pos.y * texinfo->vT.y + pos.z * texinfo->vT.z + texinfo->fTShift);

			if (s > maxtex[i].x)
				maxtex[i].x = s;
			if (s < mintex[i].x)
				mintex[i].x = s;
			
			if (t > maxtex[i].y)
				maxtex[i].y = t;
			if (t < mintex[i].y)
				mintex[i].y = t;
		}

		if (face->nLightmapOffset < 0)
		{
			int texdata = 0xFFFFFFFF;

			char name[14];
			sprintf(name, "lightmap%d", i);
			lightmaptextures[i] = AssetManager::getInst().setTexture(name, "map");
			
			glBindTexture(GL_TEXTURE_2D, lightmaptextures[i]);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata);

			continue; 
		}

		int luxelsx = (int) ceilf(maxtex[i].x / BSP_LIGHTMAP_LUXELLEN) - (int) floor(mintex[i].x / BSP_LIGHTMAP_LUXELLEN) + 1;
		int luxelsy = (int) ceilf(maxtex[i].y / BSP_LIGHTMAP_LUXELLEN) - (int) floor(mintex[i].y / BSP_LIGHTMAP_LUXELLEN) + 1;

		int* texdata = (int*)malloc(sizeof(int) * luxelsx * luxelsy);
		for (int j = 0; j < luxelsx * luxelsy; j++)
		{
			color24_t col = lightmap[j];
			texdata[j] = 0;
			texdata[j] |= (int)col.r <<  0;
			texdata[j] |= (int)col.g <<  8;
			texdata[j] |= (int)col.b << 16;
			texdata[j] |= 0xFF000000;
		}

		char name[14];
		sprintf(name, "lightmap%d", i);
		lightmaptextures[i] = AssetManager::getInst().setTexture(name, "map");
		
		glBindTexture(GL_TEXTURE_2D, lightmaptextures[i]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, luxelsx, luxelsy, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata);

		free(texdata);
	}

	LoadEntities();
}

void BSPMap::LoadEntities()
{
	char* entitieslump = (char*)mhdr + mhdr->lump[BSP_LUMP_ENTITIES].nOffset;
	int lumplen = mhdr->lump[BSP_LUMP_ENTITIES].nLength;

	char* currentchar = entitieslump;

	while ((currentchar - entitieslump) < lumplen - 1)
	{
		std::unordered_map<std::string, std::string> keyval;

		if (currentchar[0] == '{')
			currentchar += 2;

		while (currentchar[0] != '}')
		{
			if (currentchar[0] == '\n')
			{
				currentchar++;
				continue;
			}

			char* curline = ScanLine(&currentchar);

			char* keystart; char* keyend;
			char* valstart; char* valend;

			keystart = strchr(curline, '"');
			keyend = strchr(keystart + 1, '"');

			valstart = strchr(keyend + 1, '"');
			valend = strchr(valstart + 1, '"');

			char* key = (char*)malloc(keyend - keystart);
			char* val = (char*)malloc(valend - valstart);

			memcpy(key, keystart + 1, keyend - keystart - 1);
			memcpy(val, valstart + 1, valend - valstart - 1);

			key[keyend - keystart - 1] = '\0';
			val[valend - valstart - 1] = '\0';

			keyval[std::string(key)] = std::string(val);

			free(key);
			free(val);

			free(curline);
		}

		currentchar += 2;

		if (keyval["classname"] == "func_rotating")
		{
			RotatingEntity entity(*this);

			if (keyval.find("model") != keyval.end())
				entity.SetModel(std::stoi(keyval["model"].substr(1)));

			vec3_t pos = { 0.0, 0.0, 0.0 };
			if (keyval.find("origin") != keyval.end())
			{
				std::istringstream iss(keyval["origin"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				pos.x = x;
				pos.y = y;
				pos.z = z;
				entity.position = pos;
			}

			vec3_t rot = { 0.0, 0.0, 0.0 };
			if (keyval.find("angle") != keyval.end())
			{
				std::istringstream iss(keyval["angle"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				rot.x = x;
				rot.y = y;
				rot.z = z;
				entity.rotation = pos;
			}
			
			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);

			float speed = 0.0;
			if (keyval.find("speed") != keyval.end())
				entity.SetSpeed(std::stoi(keyval["speed"]));


			entity.Init();
			entities.push_back(std::make_unique<RotatingEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
		else if (keyval["classname"] == "func_wall")
		{
			WallEntity entity(*this);

			if (keyval.find("model") != keyval.end())
				entity.SetModel(std::stoi(keyval["model"].substr(1)));

			vec3_t pos = { 0.0, 0.0, 0.0 };
			if (keyval.find("origin") != keyval.end())
			{
				std::istringstream iss(keyval["origin"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				pos.x = x;
				pos.y = y;
				pos.z = z;
				entity.position = pos;
			}

			vec3_t rot = { 0.0, 0.0, 0.0 };
			if (keyval.find("angle") != keyval.end())
			{
				std::istringstream iss(keyval["angle"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				rot.x = x;
				rot.y = y;
				rot.z = z;
				entity.rotation = pos;
			}

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);


			entity.Init();
			entities.push_back(std::make_unique<WallEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
	}
}

char* BSPMap::ScanLine(char** line)
{
	char* start = *line;
	char* counter = start;

	while (counter[0] != '\n')
		counter++;

	int linelen = counter - start;

	char* output = (char*) malloc(linelen + 1);
	memcpy(output, start, linelen);
	output[linelen] = '\0';

	*line += linelen + 1;

	return output;
}

void BSPMap::SetCameraPosition(vec3_t pos)
{
	camerapos = pos;
}

void BSPMap::Draw()
{
	bspmodel_t* worldmodel = (bspmodel_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_MODELS].nOffset);
	RenderNode(worldmodel->iHeadnodes[0]);
}

void BSPMap::Think(float deltatime)
{
	for (int i = 0; i < entities.size(); i++)
		entities[i]->Think(deltatime);
}

void BSPMap::RenderNode(short nodenum)
{
	if (nodenum < 0)
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

	std::vector<int> entityindices = leafentities[leafnum];
	for(int i = 0; i < entityindices.size(); i++)
		entities[entityindices[i]]->Render();

	//for (int i = leaf->iFirstMarkSurface; i < leaf->nMarkSurfaces + leaf->iFirstMarkSurface; i++)
	//	RenderFace(marksurfaces[i]);
}

void BSPMap::RenderFace(uint16_t f)
{
	bspplane_t* planes = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset);
	bspface_t* faces = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset);
	bspface_t* face = faces + f;
	vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
	bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset) + face->iTextureInfo;
	uint32_t* texoffsets = (uint32_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset + sizeof(uint32_t));

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gltextures[texinfo->iMiptex]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lightmaptextures[f]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	miptex_t* miptex = (miptex_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset + texoffsets[texinfo->iMiptex]);
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

		float s = pos.x * texinfo->vS.x + pos.y * texinfo->vS.y + pos.z * texinfo->vS.z + texinfo->fSShift;
		s /= miptex->width;
		float t = pos.x * texinfo->vT.x + pos.y * texinfo->vT.y + pos.z * texinfo->vT.z + texinfo->fTShift;
		t /= miptex->height;

		// https://www.gamedev.net/forums/topic.asp?topic_id=538713
		
		vec2_t lightmapCoords{};
		lightmapCoords.x = s * miptex->width;
		lightmapCoords.y = t * miptex->height;

		int luxelsx = (int) ceilf(maxtex[f].x / BSP_LIGHTMAP_LUXELLEN) - (int) floor(mintex[f].x / BSP_LIGHTMAP_LUXELLEN) + 1;
		int luxelsy = (int) ceilf(maxtex[f].y / BSP_LIGHTMAP_LUXELLEN) - (int) floor(mintex[f].y / BSP_LIGHTMAP_LUXELLEN) + 1;

		float midfaceu = (mintex[f].x + maxtex[f].x) / 2.0;
		float midfacev = (mintex[f].y + maxtex[f].y) / 2.0;
		float midtexu = luxelsx / 2.0;
		float midtexv = luxelsy / 2.0;

		lightmapCoords.x = midtexu + (lightmapCoords.x - midfaceu) / BSP_LIGHTMAP_LUXELLEN;
		lightmapCoords.y = midtexv + (lightmapCoords.y - midfacev) / BSP_LIGHTMAP_LUXELLEN;

		lightmapCoords.x /= luxelsx;
		lightmapCoords.y /= luxelsy;

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

void BSPMap::SetEntityToLeaf(int entity, int leaf)
{
	if (entity < entities.size()) 
	{
		if (entityleaves.find(entity) != entityleaves.end()) 
		{
			int currentLeafIndex = entityleaves[entity];
			auto& currentLeafEntities = leafentities[currentLeafIndex];
			currentLeafEntities.erase
			(
				std::remove(currentLeafEntities.begin(), currentLeafEntities.end(), entity),
				currentLeafEntities.end()
			);
		}

		leafentities[leaf].push_back(entity);
		entityleaves[entity] = leaf;
	}
}

int BSPMap::GetLeafFromPoint(vec3_t p, int nodenum)
{
	if (nodenum < 0)
		return ~nodenum;

	bspnode_t* node = (bspnode_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_NODES].nOffset) + nodenum;
	bspplane_t* plane = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset) + node->iPlane;

	float side = plane->vNormal.x * p.x + plane->vNormal.y * p.y + plane->vNormal.z * p.z - plane->fDist;
	int firstchild = side < 0;

	return GetLeafFromPoint(p, node->iChildren[firstchild]);
}

BSPMap::~BSPMap()
{
	free(gltextures);
	free(lightmaptextures);
}