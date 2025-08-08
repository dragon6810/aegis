#include "Entity.h"

void Entity::ExpandBrushes(const HullDef& hulls)
{
    int i, j;

    for(i=0; i<Bsplib::n_hulls; i++)
    {
        if(i)
        {
            assert(!this->brushes[i].size() && "Attempint to expand already expanded entity!");
            this->brushes[i] = this->brushes[0];
        }

        for(j=0; j<this->brushes[i].size(); j++)
            this->brushes[i][j].Expand(hulls.hulls[i]);
    }
}