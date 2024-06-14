#pragma once

#include "defs.h";

class BSPMap
{
public:
	void Load(const char* filename);
	void SetCameraPosition(vec3_t pos);
	void Draw();

	~BSPMap();

	vec3_t camerapos = { 0.0, 0.0, 0.0 };
	bspheader_t* mhdr;
private:
	void RenderNode(short nodenum);
	void RenderLeaf(short leafnum);
	void RenderFace(uint16_t f);

	int* gltextures;
	int* lightmaptextures;
	vec2_t maxtex[BSP_MAX_MAP_FACES];
	vec2_t mintex[BSP_MAX_MAP_FACES];
};