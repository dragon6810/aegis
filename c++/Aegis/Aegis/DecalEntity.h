#pragma once

#include "BaseEntity.h"

#include "Wad.h"

#include <GL/glew.h>

#define DECAL_DISTANCE 2

class DecalEntity : public BaseEntity
{
public:
	DecalEntity(BSPMap& map);
	~DecalEntity();
	
	void SetTexture(char* texname);
	void SetWad(Wad& wad);

	virtual void Init() override;
	virtual void Render() override;
protected:
	typedef struct
	{
		vec3_t pos;
		vec2_t uv;
	} decalvertex_t;

	Wad* decalswad;

	std::vector<std::vector<vec3_t>> faces;
	std::vector<int> faceindices;
	std::vector<std::vector<vec2_t>> texcoords;
	std::vector<std::vector<vec2_t>> lightmapcoords;
	char texture[BSP_MAXTEXTURENAME];
	int size = 16 >> 1;
	int texwidth;
	int texheight;
	GLuint texindex;

	float dx;
	float dy;

	void RecursiveFindFaces(int nodenum);
	void AddFaces(int nodenum);
};

