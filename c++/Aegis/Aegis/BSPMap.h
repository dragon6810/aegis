#pragma once

#include "defs.h";

#include <vector>
#include <unordered_map>
#include <memory>

#include "BaseEntity.h"

class BaseEntity;

class BSPMap
{
public:
	void Load(const char* filename);
	void LoadEntities();
	char* ScanLine(char** line);
	void SetCameraPosition(vec3_t pos);
	void Draw();
	void Think(float deltatime);

	~BSPMap();

	vec3_t camerapos = { 0.0, 0.0, 0.0 };
	bspheader_t* mhdr;
	
	void RenderNode(short nodenum);
	void RenderLeaf(short leafnum);
	void RenderFace(uint16_t f);

	int* gltextures;
	int* lightmaptextures;
	vec2_t maxtex[BSP_MAX_MAP_FACES];
	vec2_t mintex[BSP_MAX_MAP_FACES];

	std::vector<std::unique_ptr<BaseEntity>> entities;
	std::unordered_map<int, std::vector<int>> leafentities;
	std::unordered_map<int, int> entityleaves;

	void SetEntityToLeaf(int entity, int leaf);
	int GetLeafFromPoint(vec3_t p, int nodenum);
};