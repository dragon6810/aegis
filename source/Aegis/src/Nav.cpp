#include "Nav.h"

#include "World.h"

#include "Console.h"

void Nav::DrawSurf(navnode_t* surf)
{
    int i;
    Vector3 cur, next;

    glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -0.1f);
    glColor4f(0.5, 0.8, 1, 0.5);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBegin(GL_POLYGON);
    
    for(i=0; i<surf->points.size(); i++)
        glVertex3f(surf->points[i].x, surf->points[i].y, surf->points[i].z);
    
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0, 0, 0);
    glLineWidth(3);
    glPolygonOffset(-2.0f, -0.3f);
    glEnable(GL_POLYGON_OFFSET_LINE);

#if 0
    glBegin(GL_LINES);
    for(i=0; i<surf->points.size(); i++)
    {
        cur = surf->points[i];
        next = surf->points[(i+1)%surf->points.size()];
        glVertex3f(cur.x, cur.y, cur.z);
        glVertex3f(next.x, next.y, next.z);
    }
    glEnd();
#else
    glBegin(GL_POLYGON);
    for(i=0; i<surf->points.size(); i++)
        glVertex3f(surf->points[i].x, surf->points[i].y, surf->points[i].z);
    glEnd();
#endif

    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glColor3f(1, 1, 1);
}

std::vector<std::array<Vector3, 3>> Nav::EarClip(std::vector<Vector3> poly)
{
    int cur, last, next;

    std::vector<std::array<Vector3, 3>> tris;
    std::array<Vector3, 3> tri;

    if(poly.size() < 3)
    {
        Console::Print("WARNING: Attempting to triangulate a polygon with only %d vertices.\n", poly.size());
        return {};
    }

    while(poly.size() > 3)
    {
        cur = 0;
        last = poly.size() - 1;
        next = 1;

        tri[0] = poly[last];
        tri[1] = poly[cur];
        tri[2] = poly[next];

        tris.push_back(tri);
        poly.erase(poly.begin()); // Remove cur
    }

    tri[0] = poly[0];
    tri[1] = poly[1];
    tri[2] = poly[2];
    tris.push_back(tri);

    return tris;
}

bool Nav::SurfQualifies(struct surf_t* surf)
{
    float maxangle, maxcos;

    maxangle = atanf(maxslope);
    maxcos = cosf(maxangle);

    return Vector3::Dot(surf->pl->n, Vector3(0, 0, 1)) > maxcos;
}

void Nav::NavSurfsFromSurf(struct surf_t* surf)
{
    int i, j;

    std::vector<Vector3> vertices;
    std::vector<std::array<Vector3, 3>> tris;

    navnode_t node;

    vertices.resize(surf->vertices.size());
    for(i=0; i<surf->vertices.size(); i++)
        vertices[i] = *surf->vertices[i];

    tris = EarClip(vertices);

    for(i=0; i<tris.size(); i++)
    {
        node.points.resize(3);
        node.points[0] = tris[i][0];
        node.points[1] = tris[i][1];
        node.points[2] = tris[i][2];
        node.normal = surf->pl->n;
        surfs.push_back(node);
    }
}

void Nav::FindSurfs(void)
{
    int i;

    for(i=0; i<world->surfs.size(); i++)
    {
        if(!SurfQualifies(&world->surfs[i]))
            continue;

        NavSurfsFromSurf(&world->surfs[i]);
    }
}

void Nav::Render(void)
{
    int i;

    for(i=0; i<surfs.size(); i++)
        DrawSurf(&surfs[i]);
}

void Nav::Initialize(World* world)
{
    this->world = world;

    FindSurfs();
}