#include "Nav.h"

#include "World.h"

#include <math.h>

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
    
    glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(0.7, 0, 0);
    
    glBegin(GL_LINES);
    for(i=0; i<surf->edges.size(); i++)
    {
        cur = surf->center + surf->normal * 2;
        next = surf->edges[i]->center + surf->normal * 2;

        glVertex3f(cur[0], cur[1], cur[2]);
        glVertex3f(next[0], next[1], next[2]);
    }
    glEnd();

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
void Nav::FindEdges(int hull)
{
    int i, j, cur, next, _cur, _next;
    bool found;

    for(i=0; i<surfs[hull].size(); i++)
    {
        for(j=i+1; j<surfs[hull].size(); j++)
        {
            found = false;

            for(cur=0; cur<surfs[hull][i].points.size(); cur++)
            {
                next = (cur + 1) % surfs[hull][i].points.size();

                for(_cur=0; _cur<surfs[hull][j].points.size(); _cur++)
                {
                    _next = (_cur + 1) % surfs[hull][j].points.size();

                    if(!SameEdge(surfs[hull][i].points[cur], surfs[hull][i].points[next], surfs[hull][j].points[_cur], surfs[hull][j].points[_next]))
                        continue;

                    surfs[hull][i].edges.push_back(&surfs[hull][j]);
                    surfs[hull][j].edges.push_back(&surfs[hull][i]);

                    found = true;
                    break;
                }

                if(found)
                    break;
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

    while(poly.size() >= 3)
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

    return tris;
}

bool Nav::SurfQualifies(navnode_t* node)
{
    float maxangle, maxcos;

    maxangle = atanf(maxslope);
    maxcos = cosf(maxangle);

    return Vector3::Dot(node->normal, Vector3(0, 0, 1)) > maxcos;
}

std::vector<navnode_t> Nav::Expand(int hull)
{
    int i, j, k;
    navnode_t curnode;

    std::vector<navnode_t> nodes;
    std::vector<navnode_t> realnodes;
    std::vector<Vector3> vertices;
    std::vector<plane_t> planes;
    std::vector<surf_t> surfs;
    std::vector<std::array<Vector3, 3>> tris;
    std::vector<std::vector<bool>> vplanes;
    Vector3 offset;

    vertices = world->verts;
    surfs = world->surfs;
    planes = world->planes;
    vplanes.resize(vertices.size());
    for(i=0; i<vplanes.size(); i++)
        vplanes[i].resize(planes.size());
    for(i=0; i<surfs.size(); i++)
    {
        offset = Vector3();
        for(j=0; j<3; j++)
        {
            if(planes[surfs[i].ipl].n[j] == 0)
                continue;

            if(planes[surfs[i].ipl].n[j] < 0)
                k = 0;
            if(planes[surfs[i].ipl].n[j] > 0)
                k = 1;

            offset[j] = World::hulls[hull][k][j];
        }

        for(j=0; j<surfs[i].ivertices.size(); j++)
        {
            if(vplanes[surfs[i].ivertices[j]][surfs[i].ipl])
                continue;
            vplanes[surfs[i].ivertices[j]][surfs[i].ipl] = true;

            vertices[surfs[i].ivertices[j]] = vertices[surfs[i].ivertices[j]] + planes[surfs[i].ipl].n * Vector3::Dot(offset, planes[surfs[i].ipl].n);
        }
    }

    nodes.resize(surfs.size());
    for(i=0; i<surfs.size(); i++)
    {
        nodes[i].points.resize(surfs[i].vertices.size());
        for(j=0; j<nodes[i].points.size(); j++)
            nodes[i].points[j] = vertices[surfs[i].ivertices[j]];
        nodes[i].normal = planes[surfs[i].ipl].n;
    }

    for(i=0; i<nodes.size(); i++)
    {
        if(!SurfQualifies(&nodes[i]))
            continue;

        tris = EarClip(nodes[i].points);
        for(j=0; j<tris.size(); j++)
        {
            curnode.normal = nodes[i].normal;
            curnode.edges.clear();

            curnode.center = Vector3(0, 0, 0);
            curnode.points.resize(3);
            for(k=0; k<3; k++)
            {
                curnode.points[k] = tris[j][k];
                curnode.center = curnode.center + tris[j][k];
            }
            curnode.center = curnode.center / 3.0;
            realnodes.push_back(curnode);
        }
    }

    return realnodes;
}

void Nav::FindSurfs(int hull)
{
    int i;

    surfs[hull] = Expand(hull);
}

void Nav::Render(void)
{
    int i;

    for(i=0; i<surfs[1].size(); i++)
        DrawSurf(&surfs[1][i]);
}

void Nav::Initialize(World* world)
{
    this->world = world;

    FindSurfs(1);
    FindEdges(1);
}
