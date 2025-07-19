#include "Entity.h"

void Entity::Draw(const Viewport& view)
{
    int i;

    for(i=0; i<this->brushes.size(); i++)
        this->brushes[i].Draw(view);
}