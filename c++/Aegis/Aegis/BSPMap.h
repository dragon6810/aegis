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

	int* gltextures;
	int* lightmaptextures;
	vec2_t* facebounds;
	vec3_t* facecenters;

	vec2_t FaceBounds(uint16_t f, vec3_t* facecenter);
	vec2_t FaceCoordinates(uint16_t f, vec3_t p);
};