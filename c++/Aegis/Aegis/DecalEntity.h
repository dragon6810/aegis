#pragma once

#include "BaseEntity.h"

#define DECAL_DISTANCE 2

class DecalEntity : public BaseEntity
{
public:
	DecalEntity(BSPMap& map);
	
	void SetTexture(char* texname);

	virtual void Init() override;
	virtual void Render() override;
protected:
	typedef struct
	{
		vec3_t pos;
		vec2_t uv;
	} decalvertex_t;

	std::vector<std::vector<vec3_t>> faces;
	char texture[BSP_MAXTEXTURENAME];
	int size = 16 >> 1;

	void RecursiveFindFaces(int nodenum);
	void AddFaces(int nodenum);
};

