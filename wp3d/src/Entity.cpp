#include "Entity.h"

#include "Map.h"

void Entity::Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, Map& map)
{
    // TODO: stop copy and pasting this code and make a general function! you know better!
    int i;

    float curdist, bestdist;
    int bestbrush;

    if(map.selectiontype == Map::SELECT_ENTITY)
    {
        if(!map.entselection.contains(index))
            map.entselection.insert(index);
        else
            map.entselection.erase(index);

        return;
    }

    bestbrush = -1;
    for(i=0; i<this->brushes.size(); i++)
    {
        if(!this->brushes[i].RayIntersect(o, r, &curdist))
            continue;

        if(bestbrush == -1 || curdist < bestdist)
        {
            bestbrush = i;
            bestdist = curdist;
        }
    }

    if(bestbrush < 0)
        return;

    this->brushes[bestbrush].Select(o, r, bestbrush, index, map);
}

void Entity::SelectVertex(Eigen::Vector3f o, Eigen::Vector3f r, const Map& map, const Viewport& view)
{
    int i;

    for(i=0; i<this->brushes.size(); i++)
    {
        if(map.selectiontype == Map::SELECT_BRUSH && !this->brselection.contains(i))
            continue;

        this->brushes[i].SelectVerts(o, r, map, view);
    }
}

bool Entity::RayIntersects(Eigen::Vector3f o, Eigen::Vector3f r, float* dist)
{
    int i;

    float curdist, bestdist;
    int bestbrush;

    bestbrush = -1;
    for(i=0; i<this->brushes.size(); i++)
    {
        if(!this->brushes[i].RayIntersect(o, r, &curdist))
            continue;

        if(bestbrush == -1 || curdist < bestdist)
        {
            bestbrush = i;
            bestdist = curdist;
        }
    }

    if(bestbrush < 0)
        return false;

    if(dist)
        *dist = bestdist;
    
    return true;
}

void Entity::Draw(const Viewport& view, int index, Map& map)
{
    int i;

    for(i=0; i<this->brushes.size(); i++)
        this->brushes[i].Draw(view, i, index, map);
}