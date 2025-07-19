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

void Brush::Draw(const Viewport& view)
{
    int i;

    for(i=0; i<this->planes.size(); i++)
        this->planes[i].Draw(view);
}