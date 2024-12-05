#include "Nav.h"

#include "World.h"

#include "Console.h"

void Nav::DrawSurf(navnode_t* surf)
{
    int i;
    Vector3 cur, next;

    glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);
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
    glPolygonOffset(-2.0f, -1.0f);
    glEnable(GL_POLYGON_OFFSET_LINE);

    glBegin(GL_POLYGON);
    for(i=0; i<surf->points.size(); i++)
        glVertex3f(surf->points[i].x, surf->points[i].y, surf->points[i].z);
    glEnd();

    glColor3f(0.7, 0, 0);
    glBegin(GL_POLYGON);
    for(i=0; i<surf->edges.size(); i++)
    {
        cur = surf->center;
        next = surf->edges[i]->center;

        glVertex3f(cur[0], cur[1], cur[2]);
        glVertex3f(next[0], next[1], next[2]);
    }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glColor3f(1, 1, 1);
}

std::pair<Vector3, Vector3> Nav::MakeEdge(Vector3 a, Vector3 b)
{
    return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z) ? std::make_pair(a, b) : std::make_pair(b, a);
}

bool Nav::SameEdge(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4)
{
    if((v1 == v3) && (v2 == v4))
        return true;

    if((v1 == v4) && (v2 == v3))
        return true;

    return false;
}

// Please update use this to use a hashmap this is super slow
void Nav::FindEdges(void)
{
    int i, j, cur, next, _cur, _next;

    for(i=0; i<surfs.size(); i++)
    {
        for(cur=0; cur<surfs[i].points.size(); cur++)
        {
            next = (cur + 1) % surfs[i].points.size();
            for(j=0; j<surfs.size(); j++)
            {
                if(i == j)
                    continue;

                for(_cur=0; _cur<surfs[j].points.size(); _cur++)
                {
                    _next = (_cur + 1) % surfs[j].points.size();

                    if(!SameEdge(surfs[i].points[cur], surfs[i].points[next], surfs[j].points[_cur], surfs[j].points[_next]))
                        continue;

                    surfs[i].edges.push_back(&surfs[j]);
                }
            }
        }
    }
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
        
        for(j=0; j<3; j++)
            node.center[j] = (node.points[0][j] + node.points[1][j] + node.points[2][j]) / 3.0;

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
    FindEdges();
}