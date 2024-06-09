#pragma once

#include "defs.h";

class BSPMap
{
public:
	void Load(const char* filename);
	void SetCameraPosition(vec3_t pos);
	void Draw();

	vec3_t camerapos = { 0.0, 0.0, 0.0 };
	bspheader_t* mhdr;
private:
	void RenderNode(short nodenum);
	void RenderLeaf(short leafnum);
};