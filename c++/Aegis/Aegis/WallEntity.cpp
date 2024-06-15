#include "WallEntity.h"

#include "GL/glew.h"

#include "Quaternion.h"

WallEntity::WallEntity(BSPMap& map) : BSPModelEntity(map)
{

}

void WallEntity::Render()
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	bspmodel_t* model = (bspmodel_t*)((char*)map->mhdr + map->mhdr->lump[BSP_LUMP_MODELS].nOffset) + whichmodel;

	glPushMatrix();

	glTranslatef(model->vOrigin.x + position.x, model->vOrigin.y + position.y, model->vOrigin.z + position.z);
	glRotatef(rotation.z * RAD2DEG, 0.0, 0.0, 1.0);
	glRotatef(rotation.y * RAD2DEG, 0.0, 1.0, 0.0);
	glRotatef(rotation.x * RAD2DEG, 1.0, 0.0, 0.0);

	map->RenderNode(model->iHeadnodes[0]);

	glPopMatrix();

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_ALPHA_TEST);
}