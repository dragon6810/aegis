#include "BSPModelEntity.h"

#include <GL/glew.h>

#include "Quaternion.h"

BSPModelEntity::BSPModelEntity(BSPMap& map) : BaseEntity(map)
{

}

void BSPModelEntity::Init()
{
	return;
}

void BSPModelEntity::Render()
{
	bspmodel_t* model = (bspmodel_t*) ((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_MODELS].nOffset) + whichmodel;

	glPushMatrix();

	glTranslatef(model->vOrigin.x + position.x, model->vOrigin.y + position.y, model->vOrigin.z + position.z);
	glRotatef(rotation.z * RAD2DEG, 0.0, 0.0, 1.0);
	glRotatef(rotation.y * RAD2DEG, 0.0, 1.0, 0.0);
	glRotatef(rotation.x * RAD2DEG, 1.0, 0.0, 0.0);

	map->RenderNode(model->iHeadnodes[0]);

	glPopMatrix();
}

void BSPModelEntity::SetModel(int whichmodel)
{
	this->whichmodel = whichmodel;
}