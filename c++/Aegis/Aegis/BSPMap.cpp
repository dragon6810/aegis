#include "BSPMap.h"

#include <GL/glew.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <math.h>

#include "Game.h"

#include "binaryloader.h"
#include "loadtexture.h"
#include "AssetManager.h"

#include "RotatingEntity.h"
#include "IllusionaryEntity.h"
#include "DecalEntity.h"
#include "SpriteEntity.h"
#include "LaserEntity.h"
#include "PlayerEntity.h"

#include "Light.h"

#include "mathutils.h"
#include "collision.h"

void BSPMap::Load(const char* filename)
{
	Game::GetGame().wad.Unload();

	loadBytes(filename, (char**) &mhdr);
	
	printf("Loading Map %s (version %d).\n", filename, mhdr->nVersion);

	int numfaces = mhdr->lump[BSP_LUMP_FACES].nLength / sizeof(bspface_t);
	
	for (int i = 0; i < numfaces; i++)
	{
		lightmaptextures.push_back({});

		bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + i;
		bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset) + face->iTextureInfo;
		color24_t* lightmap = (color24_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LIGHTING].nOffset);
		vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
		bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
		int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
		
		if (face->nLightmapOffset < 0)
		{
			int texdata = 0xFFFFFFFF;

			for (int j = 0; j < BSP_FACE_NLIGHTSTYLES; j++)
			{
				lightmaptextures[i].style[j] = BSP_LIGHTMODE_NONE;

				std::string name = "lightmap_" + std::to_string(i) + std::string("_") + std::to_string(j);
				lightmaptextures[i].texture[j] = AssetManager::getInst().setTexture(name.c_str(), "map");

				glBindTexture(GL_TEXTURE_2D, lightmaptextures[i].texture[j]);
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texdata);
			}

			continue;
		}

		maxtex[i] = { -999999.9, -999999.9 };
		mintex[i] = {  999999.9,  999999.9 };

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

		color24_t* c = (color24_t*) ((char*) lightmap + face->nLightmapOffset);

		int luxelsx = (int)ceilf(maxtex[i].x / BSP_LIGHTMAP_LUXELLEN) - (int)floor(mintex[i].x / BSP_LIGHTMAP_LUXELLEN) + 1;
		int luxelsy = (int)ceilf(maxtex[i].y / BSP_LIGHTMAP_LUXELLEN) - (int)floor(mintex[i].y / BSP_LIGHTMAP_LUXELLEN) + 1;

		for(int j = 0; j < BSP_FACE_NLIGHTSTYLES; j++)
		{
			if (face->nStyles[j] == 255)
			{
				lightmaptextures[i].style[j] = BSP_LIGHTMODE_NONE;
				continue;
			}

			std::vector<int> texdata = std::vector<int>(luxelsx * luxelsy);
			for (int k = 0; k < luxelsx * luxelsy; k++)
			{
				color24_t col = *c;

				if ((int)col.r << 1 > 255)
					col.r = 255;
				else
					col.r <<= 1;
				if ((int)col.g << 1 > 255)
					col.g = 255;
				else
					col.g <<= 1;
				if ((int)col.b << 1 > 255)
					col.b = 255;
				else
					col.b <<= 1;

				texdata[k] = 0;
				texdata[k] |= (int)col.r <<  0;
				texdata[k] |= (int)col.g <<  8;
				texdata[k] |= (int)col.b << 16;
				texdata[k] |= 0xFF000000;

				c++;
			}

			lightmaptextures[i].style[j] = face->nStyles[j];

			std::string name = "lightmap_" + std::to_string(i) + std::string("_") + std::to_string(j);
			lightmaptextures[i].texture[j] = AssetManager::getInst().setTexture(name.c_str(), "map");
		
			glBindTexture(GL_TEXTURE_2D, lightmaptextures[i].texture[j]);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, luxelsx, luxelsy, 0, GL_RGBA, GL_UNSIGNED_BYTE, texdata.data());
		}
	}

	printf("Started Loading Entites\n");
	LoadEntities();
	printf("Done Loading Entites\n");

	bsptextureheader_t* texhdr = (bsptextureheader_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset);
	for (int i = 0; i < texhdr->nMipTextures; i++)
	{
		int texoffset = *((int*)(texhdr + i + 1));
		miptex_t* miptex = (miptex_t*)((char*)texhdr + texoffset);

		printf("Map uses texture \"%s\"\n", miptex->name);

		if (miptex->offsets[0] == 0 || miptex->offsets[1] == 0 || miptex->offsets[2] == 0 || miptex->offsets[3] == 0)
		{
			gltextures.push_back(Game::GetGame().wad.LoadTexture("", miptex->name));
		}
		else
		{
			gltextures.push_back(AssetManager::getInst().setTexture(miptex->name, filename));

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
}

void BSPMap::LoadEntities()
{
	Wad decalwad;
	decalwad.LoadDecals("valve/decals.wad");
	char* entitieslump = (char*)mhdr + mhdr->lump[BSP_LUMP_ENTITIES].nOffset;
	int lumplen = mhdr->lump[BSP_LUMP_ENTITIES].nLength;

	char* currentchar = entitieslump;

	while ((currentchar - entitieslump) < lumplen - 1)
	{
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

			char key[BSP_MAX_KEY];
			char val[BSP_MAX_VALUE];

			memcpy(key, keystart + 1, keyend - keystart - 1);
			memcpy(val, valstart + 1, valend - valstart - 1);

			key[keyend - keystart - 1] = '\0';
			val[valend - valstart - 1] = '\0';

			keyval[std::string(key)] = std::string(val);

			free(curline);
		}

		currentchar += 2;

		if (keyval["classname"] == "worldspawn")
		{
			if(keyval.find("sky") != keyval.end())
				sky.LoadSky((char*) keyval["skyname"].c_str());
			
			if (keyval.find("wad") != keyval.end())
			{
				std::vector<std::string> paths;

				char* wads = &(keyval["wad"])[0];
				
				char* end = wads + strlen(wads) + 1;
				for (char* c = wads; c < end; c++)
				{
					if (c[0] == ';' || c[0] == '\0')
					{
						paths.push_back("");
						paths[paths.size() - 1].resize(c - wads + 1);
						memcpy(&(paths[paths.size() - 1])[0], wads, c - wads);

						wads = c;
						c++;
					}
				}

				for (int i = 0; i < paths.size(); i++)
					Game::GetGame().wad.Load((Game::GetGame().gamedir + "/" + paths[i]).c_str());
			}
		}
		else if (keyval["classname"] == "player_camera")
		{
			vec3_t angles = { 0.0, 0.0, 0.0 };
			if (keyval.find("angles") != keyval.end())
			{
				std::istringstream iss(keyval["angles"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				angles.x = x * DEG2RAD;
				angles.y = (y + 180) * DEG2RAD;
				angles.z = z * DEG2RAD;
				Game::GetGame().camera.rotation = { angles.z, -angles.x, angles.y };
			}

			vec3_t pos = { 0.0, 0.0, 0.0 };
			if (keyval.find("origin") != keyval.end())
			{
				std::istringstream iss(keyval["origin"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				pos.x = x;
				pos.y = y;
				pos.z = z;
				Game::GetGame().camera.position = pos;
			}
		}
		else if (keyval["classname"] == "player_tank")
		{
			PlayerEntity entity(*this);

			vec3_t angles = { 0.0, 0.0, 0.0 };
			if (keyval.find("angles") != keyval.end())
			{
				std::istringstream iss(keyval["angles"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				angles.x = x * DEG2RAD;
				angles.y = y * DEG2RAD;
				angles.z = z * DEG2RAD;
				entity.rotation = angles;
			}

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

			entity.Init();
			entities.push_back(std::make_unique<PlayerEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
		else if (keyval["classname"] == "func_rotating")
		{
			RotatingEntity entity(*this);

			entity.classname = keyval["classname"];

			if (keyval.find("targetname") != keyval.end())
				entity.targetname = keyval["targetname"];

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
			if (keyval.find("angles") != keyval.end())
			{
				std::istringstream iss(keyval["angles"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				rot.x = x;
				rot.y = y;
				rot.z = z;
				entity.rotation = rot;
			}

			if (keyval.find("rendermode") != keyval.end())
				entity.renderingmode = std::stoi(keyval["rendermode"]);

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);

			if (keyval.find("speed") != keyval.end())
				entity.SetSpeed(std::stoi(keyval["speed"]));

			entity.Init();
			entities.push_back(std::make_unique<RotatingEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
		if (keyval["classname"] == "func_wall")
		{
			WallEntity entity(*this);

			entity.classname = keyval["classname"];

			if (keyval.find("targetname") != keyval.end())
				entity.targetname = keyval["targetname"];

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
			if (keyval.find("angles") != keyval.end())
			{
				std::istringstream iss(keyval["angles"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				rot.x = x;
				rot.y = y;
				rot.z = z;
				entity.rotation = rot;
			}

			if (keyval.find("rendermode") != keyval.end())
				entity.renderingmode = std::stoi(keyval["rendermode"]);

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);


			entity.Init();
			entities.push_back(std::make_unique<WallEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
		else if (keyval["classname"] == "func_illusionary")
		{
			IllusionaryEntity entity(*this);

			entity.classname = keyval["classname"];

			if (keyval.find("targetname") != keyval.end())
				entity.targetname = keyval["targetname"];

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
			if (keyval.find("angles") != keyval.end())
			{
				std::istringstream iss(keyval["angles"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				rot.x = x;
				rot.y = y;
				rot.z = z;
				entity.rotation = pos;
			}

			if (keyval.find("rendermode") != keyval.end())
				entity.renderingmode = std::stoi(keyval["rendermode"]);

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);


			entity.Init();
			entities.push_back(std::make_unique<IllusionaryEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
		else if (keyval["classname"] == "infodecal")
		{
			DecalEntity entity(*this);

			entity.classname = keyval["classname"];

			if (keyval.find("targetname") != keyval.end())
				entity.targetname = keyval["targetname"];

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

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);

			entity.SetWad(decalwad);

			if (keyval.find("texture") != keyval.end())
				entity.SetTexture((char*) keyval["texture"].c_str());

			entity.Init();
		}
		else if (keyval["classname"] == "env_sprite")
		{
			SpriteEntity entity(*this);

			entity.classname = keyval["classname"];

			if (keyval.find("targetname") != keyval.end())
				entity.targetname = keyval["targetname"];

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

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);

			if (keyval.find("model") != keyval.end())
				entity.LoadTexture((char*)keyval["model"].c_str());

			if (keyval.find("scale") != keyval.end())
				entity.SetScale(std::stoi(keyval["scale"]) >> 1);

			if (keyval.find("framerate") != keyval.end())
				entity.SetFramerate(std::stof(keyval["framerate"]));

			entity.Init();
			entities.push_back(std::make_unique<SpriteEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
		else if (keyval["classname"] == "info_target")
		{
			BaseEntity entity(*this);

			entity.classname = keyval["classname"];

			if (keyval.find("targetname") != keyval.end())
				entity.targetname = keyval["targetname"];

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

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);

			entity.Init();
			entities.push_back(std::make_unique<BaseEntity>(entity));
			SetEntityToLeaf(entities.size() - 1, GetLeafFromPoint(pos, 0));
		}
		else if (keyval["classname"] == "env_laser")
		{
			LaserEntity entity(*this);

			entity.classname = keyval["classname"];

			if (keyval.find("targetname") != keyval.end())
				entity.targetname = keyval["targetname"];

			if (keyval.find("LaserTarget") != keyval.end())
				entity.target = keyval["LaserTarget"];

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

			vec3_t col = { 0.0, 0.0, 0.0 };
			if (keyval.find("rendercolor") != keyval.end())
			{
				std::istringstream iss(keyval["rendercolor"]);
				int x; int y; int z;
				iss >> x >> y >> z;
				col.x = (float) x / 255.0;
				col.y = (float) y / 255.0;
				col.z = (float) z / 255.0;
				entity.color = col;
			}

			if (keyval.find("renderamt") != keyval.end())
				entity.brightness = (float)std::stoi(keyval["renderamt"]) / 255.0;

			if (keyval.find("TextureScroll") != keyval.end())
				entity.scrollspeed = (float)std::stoi(keyval["TextureScroll"]) / 10.0;

			if (keyval.find("width") != keyval.end())
				entity.width = (float)std::stoi(keyval["width"]) * 0.1;

			if (keyval.find("NoiseAmplitude") != keyval.end())
				entity.scale = (float)std::stoi(keyval["NoiseAmplitude"]);

			if (keyval.find("spawnflags") != keyval.end())
				entity.flags = std::stoi(keyval["spawnflags"]);

			entity.Init();
			entities.push_back(std::make_unique<LaserEntity>(entity));
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
	if (!mhdr)
		return;

	markleaves.clear();
	markfaces.clear();
	markentities.clear();
	_markleaves.clear();
	_markfaces.clear();
	_markentities.clear();

	bspmodel_t* worldmodel = (bspmodel_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_MODELS].nOffset);
	cameraleaf = GetLeafFromPoint(Game::GetGame().camera.position, worldmodel->iHeadnodes[0]);
	
	RenderLeaves();
	RenderFaces();
	RenderEntities();
}

void BSPMap::Think(float deltatime)
{
	for (int i = 0; i < entities.size(); i++)
		entities[i]->Think(deltatime);
}

void BSPMap::RenderLeaves()
{
	RenderLeavesRecursive(0);

	for (int i = 0; i < markleaves.size(); i++)
		RenderLeaf(markleaves[i]);
}

void BSPMap::RenderFaces()
{
	for (int i = 0; i < markfaces.size(); i++)
		RenderFace(markfaces[i]);
}

void BSPMap::RenderEntities()
{
	for (int i = markentities.size() - 1; i >= 0; i--)
	{
		entities[markentities[i]]->cameraforward = Game::GetGame().camera.forward;
		entities[markentities[i]]->cameraup = Game::GetGame().camera.up;
		entities[markentities[i]]->camerapos = Game::GetGame().camera.position;
		entities[markentities[i]]->Render();
	}
}

void BSPMap::RenderLeavesRecursive(int nodenum)
{
	if (nodenum < 0)
	{
		if (_markleaves.find(~nodenum) == _markleaves.end())
		{
			markleaves.push_back(~nodenum);
			_markleaves.insert(~nodenum);
		}
		return;
	}

	bspnode_t* node = (bspnode_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_NODES].nOffset) + nodenum;
	bspplane_t* plane = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset) + node->iPlane;

	vec3_t camerapos = Game::GetGame().camera.position;
	float side = plane->vNormal.x * camerapos.x + plane->vNormal.y * camerapos.y + plane->vNormal.z * camerapos.z - plane->fDist;
	int firstchild = side < 0;

	RenderLeavesRecursive(node->iChildren[firstchild]);

	RenderLeavesRecursive(node->iChildren[!firstchild]);
}

void BSPMap::RenderLeaf(short leafnum)
{
	bspleaf_t* leaf = (bspleaf_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LEAVES].nOffset) + leafnum;
	uint16_t* marksurfaces = (uint16_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_MARKSURFACES].nOffset);

	std::vector<int> entityindices = leafentities[leafnum];
	for (int i = 0; i < entityindices.size(); i++)
	{
		if (_markentities.find(entityindices[i]) == _markentities.end())
		{
			markentities.push_back(entityindices[i]);
			_markentities.insert(entityindices[i]);
		}
	}

	for (int i = leaf->iFirstMarkSurface; i < leaf->nMarkSurfaces + leaf->iFirstMarkSurface; i++)
	{
		if (_markfaces.find(marksurfaces[i]) == _markfaces.end())
		{
			markfaces.push_back(marksurfaces[i]);
			_markfaces.insert(marksurfaces[i]);
		}
	}
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

	miptex_t* miptex = (miptex_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset + texoffsets[texinfo->iMiptex]);
	
	if (strcmp("sky", miptex->name) == 0) // Sky is rendered beforehand, skip.
	{
		std::vector<vec3_t> points;

		for (int j = face->iFirstEdge; j < face->iFirstEdge + face->nEdges; j++)
		{
			vec3_t pos;
			if (surfedges[j] >= 0)
				pos = vertices[edges[surfedges[j]].iVertex[0]];
			else
				pos = vertices[edges[-surfedges[j]].iVertex[1]];

			points.push_back(pos);
		}

		sky.RenderFace(points);
		return;
	}

	vec3_t normal = planes[face->iPlane].vNormal;
	if (face->nPlaneSide != 0)
	{
		normal.x = -normal.x;
		normal.y = -normal.y;
		normal.z = -normal.z;
	}

	glCullFace(GL_FRONT);
	
	glEnable(GL_BLEND);

	// First Pass: Sum the lightmaps
	glActiveTexture(GL_TEXTURE0);
	for (int i = 0; i < BSP_FACE_NLIGHTSTYLES; i++)
	{
		if (lightmaptextures[f].style[i] == BSP_LIGHTMODE_NONE)
			continue;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, lightmaptextures[f].texture[i]);

		const char* style = lightpresets[lightmaptextures[f].style[i]];
		int stylelen = strlen(style);
		int index = Game::GetGame().Time() * 6.0;
		float brightness = (float)(style[index % stylelen] - 'a') / 26.0;

		glColor3f(brightness, brightness, brightness);

		if (i == 0)
			glBlendFunc(GL_ONE, GL_ZERO);
		else
			glBlendFunc(GL_ONE, GL_ONE);

		glBegin(GL_POLYGON);
		for (int j = face->iFirstEdge; j < face->iFirstEdge + face->nEdges; j++)
		{
			vec3_t pos;
			if (surfedges[j] >= 0)
				pos = vertices[edges[surfedges[j]].iVertex[0]];
			else
				pos = vertices[edges[-surfedges[j]].iVertex[1]];

			vec2_t lightmapCoords = GetLightmapCoords(f, pos);

			glTexCoord2f(lightmapCoords.x, lightmapCoords.y);
			glVertex3f(pos.x, pos.y, pos.z);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	glColor3f(1, 1, 1);

	// Second Pass: Multiply the summed lightmaps with the base texture
	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, gltextures[texinfo->iMiptex]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

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

		glTexCoord2f(s, t);
		glVertex3f(pos.x, pos.y, pos.z);
	}
	glEnd();

	glDisable(GL_BLEND);

	for (int i = 0; i < BSP_FACE_NLIGHTSTYLES; i++)
	{
		if (lightmaptextures[f].style[i] == BSP_LIGHTMODE_NONE)
			continue;

		glActiveTexture(GL_TEXTURE0 + i);
		glDisable(GL_TEXTURE_2D);
	}

	for (int i = 0; i < facedecals[f].size(); i++)
		facedecals[f][i]->Render();
}

bool BSPMap::IsLeafVisible(int leaf1, int leaf2)
{
	int numleaves = mhdr->lump[BSP_LUMP_LEAVES].nLength / sizeof(bspleaf_t);
	int index = leaf1 * numleaves + leaf2;
	int byte = index >> 3;
	int bit = index & 7;

	ubyte_t* vislump = (ubyte_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VISIBILITY].nOffset);
	ubyte_t val = vislump[byte];
	val >>= bit;
	val &= 1;

	return val;
}

// https://www.gamedev.net/forums/topic.asp?topic_id=538713
vec2_t BSPMap::GetLightmapCoords(uint16_t f, vec3_t pos)
{
	bspplane_t* planes = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset);
	bspface_t* faces = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset);
	bspface_t* face = faces + f;
	vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
	bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
	int* surfedges = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset);
	bsptexinfo_t* texinfo = (bsptexinfo_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXINFO].nOffset) + face->iTextureInfo;
	uint32_t* texoffsets = (uint32_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset + sizeof(uint32_t));

	miptex_t* miptex = (miptex_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_TEXTURES].nOffset + texoffsets[texinfo->iMiptex]);

	vec2_t lightmapCoords{};
	lightmapCoords.x = pos.x * texinfo->vS.x + pos.y * texinfo->vS.y + pos.z * texinfo->vS.z + texinfo->fSShift;
	lightmapCoords.y = pos.x * texinfo->vT.x + pos.y * texinfo->vT.y + pos.z * texinfo->vT.z + texinfo->fTShift;

	int luxelsx = (int)ceilf(maxtex[f].x / BSP_LIGHTMAP_LUXELLEN) - (int)floor(mintex[f].x / BSP_LIGHTMAP_LUXELLEN) + 1;
	int luxelsy = (int)ceilf(maxtex[f].y / BSP_LIGHTMAP_LUXELLEN) - (int)floor(mintex[f].y / BSP_LIGHTMAP_LUXELLEN) + 1;

	float midfaceu = (mintex[f].x + maxtex[f].x) / 2.0;
	float midfacev = (mintex[f].y + maxtex[f].y) / 2.0;
	float midtexu = luxelsx / 2.0;
	float midtexv = luxelsy / 2.0;

	lightmapCoords.x = midtexu + (lightmapCoords.x - midfaceu) / BSP_LIGHTMAP_LUXELLEN;
	lightmapCoords.y = midtexv + (lightmapCoords.y - midfacev) / BSP_LIGHTMAP_LUXELLEN;

	lightmapCoords.x /= luxelsx;
	lightmapCoords.y /= luxelsy;

	return lightmapCoords;
}

int BSPMap::LeafContents(vec3_t point)
{
	return LeafContentsRecursive(point, 0);
}

int BSPMap::LeafContentsRecursive(vec3_t point, int iclipnode)
{
	if (iclipnode < 0)
		return iclipnode;

	bspclipnode_t* clipnode = (bspclipnode_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_CLIPNODES].nOffset) + iclipnode;
	bspplane_t* plane = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset) + clipnode->iPlane;

	float t = DotProduct(plane->vNormal, point) - plane->fDist;
	if (t >= 0)
		return LeafContentsRecursive(point, clipnode->iChildren[0]);
	return LeafContentsRecursive(point, clipnode->iChildren[1]);
}

bool BSPMap::FineRaycast(vec3_t start, vec3_t end, vec3_t* intersection)
{
	return FineRaycastRecursive(start, end, intersection, 0);
}

bool BSPMap::FineRaycastRecursive(vec3_t start, vec3_t end, vec3_t* intersection, int iclipnode)
{
	if (iclipnode == BSP_CONTENTS_SOLID)
	{
		*intersection = start;
		return true;
	}
	else if (iclipnode < 0)
	{
		return false;
	}

	bspclipnode_t* clipnode = (bspclipnode_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_CLIPNODES].nOffset) + iclipnode;
	bspplane_t* plane = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset) + clipnode->iPlane;

	float t1 = DotProduct(plane->vNormal, start) - plane->fDist;
	float t2 = DotProduct(plane->vNormal, end) - plane->fDist;

	if (t1 * t2 >= 0)
	{
		if (t1 >= 0)
			return FineRaycastRecursive(start, end, intersection, clipnode->iChildren[0]);
		else
			return FineRaycastRecursive(start, end, intersection, clipnode->iChildren[1]);
	}

	float t;
	if (t1 < 0)
		t = (t1 + COLLIDE_EPSILON) / (t1 - t2);
	else
		t = (t1 - COLLIDE_EPSILON) / (t1 - t2);

	if (t < 0)
		t = 0;
	if (t > 1)
		t = 1;
	
	vec3_t mid = Vector3Lerp(start, end, t);
	int side = (t1 >= 0) ? 0 : 1;

	if (FineRaycastRecursive(start, mid, intersection, clipnode->iChildren[side]))
		return true;
	return FineRaycastRecursive(mid, end, intersection, clipnode->iChildren[side]);
}

vec3_t BSPMap::LightColor(vec3_t start, vec3_t end)
{
	vec3_t col = { 0.0, 0.0, 0.0 };
	LightColorRecursive(start, end, 0, &col);
	return col;
}

bool BSPMap::LightColorRecursive(vec3_t start, vec3_t end, int nodenum, vec3_t* color)
{
	if (nodenum < 0)
		return false;

	bspnode_t* node = (bspnode_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_NODES].nOffset) + nodenum;
	bspplane_t* plane = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset) + node->iPlane;

	float startval = DotProduct(start, plane->vNormal) - plane->fDist;
	float endval = DotProduct(end, plane->vNormal) - plane->fDist;

	vec3_t difference = end - start;
	float mid = (plane->fDist - DotProduct(plane->vNormal, start)) / DotProduct(plane->vNormal, difference);
	vec3_t midpoint = start + difference * mid;

	if (endval * startval < 0.0 || ((startval == 0) ^ (endval == 0))) // They are on different sides
	{
		for (int i = node->firstFace; i < node->firstFace + node->nFaces; i++)
		{
			bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + i;

			std::vector<vec3_t> polygon;
			bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
			vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);
			for (int j = face->iFirstEdge; j < face->iFirstEdge + face->nEdges; j++)
			{
				int surfedge = ((int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset))[j];

				vec3_t pos;
				if (surfedge >= 0)
					pos = vertices[edges[ surfedge].iVertex[0]];
				else
					pos = vertices[edges[-surfedge].iVertex[1]];

				polygon.push_back(pos);
			}
			
			if (IsPointInPolygon(midpoint, polygon, plane->vNormal))
			{
				color24_t* lightmap = (color24_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LIGHTING].nOffset + face->nLightmapOffset);

				int luxelsx = (int)ceilf(maxtex[i].x / BSP_LIGHTMAP_LUXELLEN) - (int)floor(mintex[i].x / BSP_LIGHTMAP_LUXELLEN) + 1;
				int luxelsy = (int)ceilf(maxtex[i].y / BSP_LIGHTMAP_LUXELLEN) - (int)floor(mintex[i].y / BSP_LIGHTMAP_LUXELLEN) + 1;

				vec2_t coords = GetLightmapCoords(i, midpoint);
				coords.x *= luxelsx;
				coords.y *= luxelsy;

				*color = { 0, 0, 0 };
				for (int j = 0; j < BSP_FACE_NLIGHTSTYLES; j++)
				{
					if (face->nStyles[j] == 255)
						continue;

					const char* style = lightpresets[face->nStyles[j]];
					int stylelen = strlen(style);
					int index = Game::GetGame().Time() * 6.0;
					float brightness = (float)(style[index % stylelen] - 'a') / 13.0;

					int texindex = (int)coords.y * luxelsx + (int)coords.x;
					vec3_t add = { (float)lightmap[texindex].r * brightness, (float)lightmap[texindex].g * brightness, (float)lightmap[texindex].b * brightness };
					*color = *color + add;

					lightmap += luxelsx * luxelsy;
				}

				return true;
			}
		}

		int firstside = startval < 0;

		bool found = LightColorRecursive(start, midpoint, node->iChildren[firstside], color);

		if (!found)
			return LightColorRecursive(midpoint, end, node->iChildren[!firstside], color);
		else
			return true;
	}
	else if (endval == 0 && startval == 0)
	{
		bool found = LightColorRecursive(start, end, node->iChildren[0], color);

		if (!found)
			return LightColorRecursive(start, end, node->iChildren[1], color);
		else
			return true;
	}

	return LightColorRecursive(start, end, node->iChildren[startval <= 0], color);
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

void BSPMap::BoxIntersect(vec3_t bmin, vec3_t bmax, int nodenum, std::vector<int>& faces)
{
	if (nodenum < 0)
	{
		nodenum = ~nodenum;
		bspleaf_t* leaf = (bspleaf_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_LEAVES].nOffset) + nodenum;

		for (int i = leaf->iFirstMarkSurface; i < leaf->nMarkSurfaces + leaf->iFirstMarkSurface; i++)
		{
			uint16_t* marksurface = (uint16_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_MARKSURFACES].nOffset) + i;
			bspface_t* face = (bspface_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_FACES].nOffset) + *marksurface;
			bspedge_t* edges = (bspedge_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_EDGES].nOffset);
			vec3_t* vertices = (vec3_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_VERTICES].nOffset);

			std::vector<vec3_t> poly;
			for (int e = face->iFirstEdge; e < face->iFirstEdge + face->nEdges; e++)
			{
				int* surfedge = (int*)((char*)mhdr + mhdr->lump[BSP_LUMP_SURFEDGES].nOffset) + e;

				vec3_t pos{};
				if (*surfedge >= 0)
					pos = vertices[edges[*surfedge].iVertex[0]];
				else
					pos = vertices[edges[*surfedge].iVertex[1]];

				poly.push_back(pos);
			}

			poly = BoxFace(bmin, bmax, poly);
			if (poly.size() > 0)
				faces.push_back(*marksurface);
		}

		return;
	}

	bspnode_t* node = (bspnode_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_NODES].nOffset) + nodenum;
	bspplane_t* plane = (bspplane_t*)((char*)mhdr + mhdr->lump[BSP_LUMP_PLANES].nOffset) + node->iPlane;

	vec3_t v[8]{};
	v[0] = { bmin.x, bmin.y, bmin.z };
	v[1] = { bmin.x, bmin.y, bmax.z };
	v[2] = { bmin.x, bmax.y, bmin.z };
	v[3] = { bmin.x, bmax.y, bmax.z };
	v[4] = { bmax.x, bmin.y, bmin.z };
	v[5] = { bmax.x, bmin.y, bmax.z };
	v[6] = { bmax.x, bmax.y, bmin.z };
	v[7] = { bmax.x, bmax.y, bmax.z };

	int side = 2; // 1 for front, 0 for both, -1 for back

	for (int i = 0; i < 8; i++) 
	{
		vec3_t p = v[i];
		float pside = plane->vNormal.x * p.x + plane->vNormal.y * p.y + plane->vNormal.z * p.z - plane->fDist;

		if (side == 2)
		{
			if (pside > 0)
				side = 1;
			else if (pside < 0)
				side = -1;
			else
			{
				side = 0;
				break;
			}
		}
		else if (pside > 0) 
		{
			if (side == -1) 
			{
				side = 0; // Intersecting the plane
				break;
			}
			side = 1; // Front side
		}
		else if (pside < 0) 
		{
			if (side == 1) 
			{
				side = 0; // Intersecting the plane
				break;
			}
			side = -1; // Back side
		}
		else
		{
			side = 0; // On the plane
			break;
		}
	}

	if (side == 1)
		BoxIntersect(bmin, bmax, node->iChildren[0], faces);
	else if (side == -1)
		BoxIntersect(bmin, bmax, node->iChildren[1], faces);
	else
	{
		BoxIntersect(bmin, bmax, node->iChildren[0], faces);
		BoxIntersect(bmin, bmax, node->iChildren[1], faces);
	}
}

BSPMap::~BSPMap()
{
	for (int i = 0; i < entities.size(); i++)
		entities[i].release();
}