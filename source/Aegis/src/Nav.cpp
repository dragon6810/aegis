#include "Nav.h"

#include "World.h"

void Nav::DrawSurf(struct surf_t* surf)
{
    int i;

    glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -0.1f);
    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
    
    for(i=0; i<surf->vertices.size(); i++)
        glVertex3f(surf->vertices[i]->x, surf->vertices[i]->y, surf->vertices[i]->z);
    
    glEnd();
    glColor3f(1, 1, 1);
    glDisable(GL_POLYGON_OFFSET_FILL);
}

bool Nav::SurfQualifies(struct surf_t* surf)
{
    float maxangle, maxcos;

    maxangle = atanf(maxslope);
    maxcos = cosf(maxangle);


    return Vector3::Dot(surf->pl->n, Vector3(0, 0, 1)) > maxcos;
}

void Nav::FindSurfs(void)
{
    int i;

    for(i=0; i<world->surfs.size(); i++)
    {
        if(!SurfQualifies(&world->surfs[i]))
            continue;

        surfs.push_back(&world->surfs[i]);
    }
}

void Nav::Render(void)
{
    int i;

    for(i=0; i<surfs.size(); i++)
        DrawSurf(surfs[i]);
}

void Nav::Initialize(World* world)
{
    this->world = world;

    FindSurfs();
}