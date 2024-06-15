#include "BSPModelEntity.h"

#include <GL/glew.h>

BSPModelEntity::BSPModelEntity(BSPMap& map) : BaseEntity(map)
{

}

void BSPModelEntity::Render()
{
	bspmodel_t* model = (bspmodel_t*) ((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_MODELS].nOffset) + whichmodel;

	glTranslatef(model->vOrigin.x + position.x, model->vOrigin.y + position.y, model->vOrigin.z + position.z);

	map->RenderNode(model->iHeadnodes[0]);

	glTranslatef(-model->vOrigin.x - position.x, -model->vOrigin.y - position.y, -model->vOrigin.z - position.z);
}

void BSPModelEntity::SetModel(int whichmodel)
{
	this->whichmodel = whichmodel;
}