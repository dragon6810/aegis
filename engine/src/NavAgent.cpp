#include "NavAgent.h"

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <set>

#include <GL/glew.h>

std::vector<navnode_t*> NavAgent::AStar(NavMesh* mesh, navnode_t* start, navnode_t* end)
{
    int i;

    assert(mesh);
    assert(start);
    assert(end);

    std::vector<navnode_t*> frontier;
    std::vector<navnode_t*> smallest;
    std::vector<navnode_t*> path;
    navnode_t *pathroot, *toexplore;
    float smallestf, smallesth;

    for(i=0; i<mesh->surfs[0].size(); i++)
    {
        mesh->surfs[0][i].exploredby = NULL;
        mesh->surfs[0][i].h = 0;
        mesh->surfs[0][i].f = 0;
        mesh->surfs[0][i].explored = false;
    }

    frontier.resize(start->edges.size());
    for(i=0; i<start->edges.size(); i++)
    {
        frontier[i] = start->edges[i];
        frontier[i]->h = (end->center - frontier[i]->center).norm();
        frontier[i]->f = (frontier[i]->center - start->center).norm() + frontier[i]->h;
        frontier[i]->exploredby = start;
        frontier[i]->explored = false;
    }
    start->h = (end->center - start->center).norm();
    start->f = start->h;

    pathroot = NULL;
    while(frontier.size())
    {
        for(i=0; i<frontier.size(); i++)
            if(!i || frontier[i]->f < smallestf)
                smallestf = frontier[i]->f;
        
        smallest.clear();
        for(i=0; i<frontier.size(); i++)
            if(frontier[i]->f == smallestf)
                smallest.push_back(frontier[i]);

        toexplore = smallest[0];
        if(smallest.size() > 1)
        {
            smallesth = toexplore->h;
            for(i=1; i<smallest.size(); i++)
            {
                if(smallest[i]->h < smallesth)
                {
                    toexplore = smallest[i];
                    smallesth = smallest[i]->h;
                }
            }
        }

        frontier.erase(std::remove(frontier.begin(), frontier.end(), toexplore), frontier.end());

        for(i=0; i<toexplore->edges.size(); i++)
        {
            if(toexplore->edges[i]->explored)
                continue;

            if(toexplore->edges[i] == end)
            {
                pathroot = toexplore;
                break;
            }
            
            //frontier.erase(std::remove(frontier.begin(), frontier.end(), toexplore->edges[i]), frontier.end());
            toexplore->edges[i]->h = (toexplore->edges[i]->center - start->center).norm();
            toexplore->edges[i]->f = (toexplore->edges[i]->center - toexplore->center).norm() + (toexplore->f - toexplore->h);
            toexplore->edges[i]->f += toexplore->edges[i]->h;
            toexplore->edges[i]->exploredby = toexplore;
            frontier.push_back(toexplore->edges[i]);
        }

        toexplore->explored = true;
    };

    // No path was found
    if(!pathroot)
        return std::vector<navnode_t*>();

    path.push_back(end);
    do
    {
        path.push_back(pathroot);
        pathroot = pathroot->exploredby;
    } while(pathroot != start);
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    return path;
}

void NavAgent::RenderPath()
{
    int i, j;

    Eigen::Vector3f v;

    glBegin(GL_POINTS);
    glColor3f(1, 1, 0);
    for(i=0; i<curpath.anchors.size(); i++)
    {
        v = curpath.anchors[i].pos;
        v = v + curpath.anchors[i].node->normal;
        glVertex3f(v[0], v[1], v[2]);
    }
    glEnd();

    if(curpath.anchors.size() > 1)
    {
        glBegin(GL_LINES);
        glColor3f(0, 1, 0);
        for(i=0; i<curpath.anchors.size()-1; i++)
        {
            for(j=0; j<2; j++)
            {
                v = curpath.anchors[i+j].pos;
                v = v + curpath.anchors[i+j].node->normal;
                glVertex3f(v[0], v[1], v[2]);
            }
        }
        glEnd();
    }

    glColor3f(1, 1, 1);
}