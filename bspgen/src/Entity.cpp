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

void Entity::CullInterior(void)
{
    int h, b1, b2;

    for(h=0; h<Bsplib::n_hulls; h++)
    {
        for(b1=0; b1<this->brushes[h].size(); b1++)
        {
            this->brushes[h][b1].PopulateExterior();
            for(b2=0; b2<this->brushes[h].size(); b2++)
            {
                if(b1 == b2 || !this->brushes[h][b1].Overlaps(this->brushes[h][b2]))
                    continue;

                this->brushes[h][b1].SeperateInOut(this->brushes[h][b2], b2 > b1);
            }
        }
    }
}