#pragma once

#include "BaseEntity.h"

#define BSP_MODEL_RENDERMODE_NORMAL 1
#define BSP_MODEL_RENDERMODE_COLOR 2
#define BSP_MODEL_RENDERMODE_TEXTURE 3
#define BSP_MODEL_RENDERMODE_SOLID 4
#define BSP_MODEL_RENDERMODE_ADDITIVE 5

class BSPModelEntity : public BaseEntity
{
public:
	BSPModelEntity(BSPMap& map);

	void SetModel(int whichmodel);

	virtual void Init() override;
	virtual void Render() override;

	int renderingmode = BSP_MODEL_RENDERMODE_NORMAL;
protected:
	int whichmodel = 0;
};

