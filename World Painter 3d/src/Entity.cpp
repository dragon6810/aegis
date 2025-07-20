#include "Entity.h"

void Entity::Draw(const Viewport& view, int index, const Map& map)
{
    int i;

    for(i=0; i<this->brushes.size(); i++)
        this->brushes[i].Draw(view, i, index, map);
}