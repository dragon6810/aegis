#include "Brush.h"

#include "Map.h"

void Brush::MakeFaces(void)
{
    int i, j;

    for(i=0; i<this->planes.size(); i++)
    {
        this->planes[i].poly.FromPlane(this->planes[i].normal, this->planes[i].d, Map::max_map_size);
        for(j=0; j<this->planes.size(); j++)
        {
            if(i == j)
                continue;

            this->planes[i].poly.Clip(this->planes[j].normal, this->planes[j].d, 0);
        }
    }
}

bool Brush::RayIntersect(Eigen::Vector3f o, Eigen::Vector3f d, float* dist)
{
    int i;

    float bestdist, curdist;

    bestdist = -1;
    for(i=0; i<this->planes.size(); i++)
    {
        if(!this->planes[i].RayIntersectFace(o, d, &curdist))
            continue;

        if(curdist < bestdist || bestdist < 0)
            bestdist = curdist;
    }

    if(bestdist < 0)
        return false;

    if(dist)
        *dist = bestdist;

    return true;
}

void Brush::Draw(const Viewport& view, int index, int ent, const Map& map)
{
    int i;

    for(i=0; i<this->planes.size(); i++)
        this->planes[i].Draw(view, i, index, ent, map);
}