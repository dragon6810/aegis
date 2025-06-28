#include "NavMesh.h"

#include "World.h"

#include <cassert>
#include <math.h>

#include "PolyMath.h"

#include "Console.h"

void NavMesh::DrawSurf(navnode_t* surf)
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

std::pair<Vector3, Vector3> NavMesh::MakeEdge(Vector3 a, Vector3 b)
{
    return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z) ? std::make_pair(a, b) : std::make_pair(b, a);
}

bool NavMesh::SameEdge(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4)
{
    if(v1 == v3)
        return v2 == v4;
    if(v1 == v4)
        return v2 == v3;

    return false;
}

// Please update use this to use a hashmap this is super slow
void NavMesh::FindEdges(int hull)
{
    int i, j, cur, next, _cur, _next;
    Vector3 *v1, *v2, *v3, *v4;
    bool found;

    for(i=0; i<surfs[hull].size()-1; i++)
    {
        for(j=i+1; j<surfs[hull].size(); j++)
        {
            found = false;

            for(cur=0; cur<surfs[hull][i].points.size(); cur++)
            {
                next = (cur + 1) % surfs[hull][i].points.size();
                v1 = &surfs[hull][i].points[cur];
                v2 = &surfs[hull][i].points[next];

                for(_cur=0; _cur<surfs[hull][j].points.size(); _cur++)
                {
                    _next = (_cur + 1) % surfs[hull][j].points.size();
                    v3 = &surfs[hull][j].points[_cur];
                    v4 = &surfs[hull][j].points[_next];

                    if(!SameEdge(*v1, *v2, *v3, *v4))
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

std::vector<std::array<Vector3, 3>> NavMesh::EarClip(std::vector<Vector3> poly)
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

bool NavMesh::SurfQualifies(const navnode_t& node)
{
    float maxangle, maxcos;

    maxangle = atanf(maxslope);
    maxcos = cosf(maxangle);

    return Vector3::Dot(node.normal, Vector3(0, 0, 1)) > maxcos;
}

std::vector<navnode_t> NavMesh::Expand(int hull)
{
    int i, j, k;
    navnode_t curnode;

    std::vector<navnode_t> nodes;
    std::vector<std::array<Vector3, 3>> tris;

    nodes.resize(world->surfs.size());
    for(i=0; i<nodes.size(); i++)
    {
        nodes[i] = {};
        nodes[i].points.resize(world->surfs[i].vertices.size());
        for(j=0; j<nodes[i].points.size(); j++)
            nodes[i].points[j] = world->verts[world->surfs[i].vertices[j]];
        nodes[i].normal = world->planes[world->surfs[i].pl].n;
        if(world->surfs[i].reverse)
            nodes[i].normal = nodes[i].normal * -1;
        nodes[i].center = PolyMath::FindCenter(nodes[i].points);
    }

    return nodes;
}

std::vector<navnode_t> NavMesh::CutPlanes(const std::vector<navnode_t>& surfs)
{
    int i, j;

    std::vector<navnode_t> newsurfs;
    std::vector<Vector3> front, back;
    Vector3 n;
    float d;
    navnode_t newnode;

    newsurfs = surfs;

    for(i=0; i<newsurfs.size(); i++)
    {
        n = newsurfs[i].normal;
        d = Vector3::Dot(n, newsurfs[i].points[0]);
        for(j=0; j<newsurfs.size(); j++)
        {
            if(i == j)
                continue;

            if(!PolyMath::PlaneCrosses(newsurfs[j].points, n, d))
                continue;

            back = PolyMath::ClipToPlane(newsurfs[j].points, n, d, false);
            front = PolyMath::ClipToPlane(newsurfs[j].points, n, d, true);

            newnode = newsurfs[j];
            newnode.points = front;
            newsurfs[j].points = back;
            newnode.center = PolyMath::FindCenter(newnode.points);
            newsurfs[j].center = PolyMath::FindCenter(newsurfs[j].points);
            
            newsurfs.insert(newsurfs.begin() + j, newnode);
            if(i > j)
                i++;
            j++;
        }
    }

    return newsurfs;
}

std::vector<navnode_t> NavMesh::PruneFaces(const std::vector<navnode_t>& surfs)
{
    int i, j;

    std::vector<navnode_t> newnodes;
    std::vector<std::array<Vector3, 3>> nodetris;
    navnode_t newnode;

    for(i=0; i<surfs.size(); i++)
    {
        if(!SurfQualifies(surfs[i]))
            continue;

        nodetris = EarClip(surfs[i].points);
        for(j=0; j<nodetris.size(); j++)
        {
            newnode = {};
            newnode.points = std::vector<Vector3>(nodetris[j].begin(), nodetris[j].end());
            newnode.center = PolyMath::FindCenter(newnode.points);
            newnode.normal = surfs[i].normal;
            newnodes.push_back(newnode);
        }
    }

    return newnodes;
}

void NavMesh::FindSurfs(int hull)
{
    int i;

    surfs[hull] = Expand(hull);
    //surfs[hull] = CutPlanes(surfs[hull]);
    //surfs[hull] = PruneFaces(surfs[hull]);
}

void NavMesh::Render(void)
{
    int i;

    for(i=0; i<surfs[0].size(); i++)
        DrawSurf(&surfs[0][i]);
}

void NavMesh::Initialize(World* world)
{
    int i, j, k;

    this->world = world;

    for(i=0; i<NHULLS*0+1; i++)
    {
        FindSurfs(i);
        FindEdges(i);
    }

    for(i=0; i<surfs[0].size()-1; i++)
    {
        for(j=i+1; j<surfs[0].size(); j++)
        {
            if(surfs[0][i].points.size() != surfs[0][j].points.size())
                continue;

            for(k=0; k<surfs[0][i].points.size(); k++)
            {
                if(surfs[0][i].points[k] != surfs[0][j].points[k])
                    break;
            }

            if(k < surfs[0][i].points.size())
                continue;

            Console::Print("duplicate\n");
        }
    }
}
