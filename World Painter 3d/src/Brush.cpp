#include "Brush.h"

#include "Map.h"

void Brush::MakeFaces(void)
{
    int i, j;

    for(i=0; i<this->planes.size(); i++)
    {
        this->planes[i].poly.points.clear();
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

void Brush::Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, int ent, Map& map)
{
    // TODO: also stop duplicating code here! what's gotten into you!?

    int i;

    std::unordered_set<int> *selection;
    float bestdist, curdist;
    int bestplane;

    if(map.selectiontype == Map::SELECT_BRUSH)
    {
        selection = &map.entities[ent].brselection;
        if(!selection->contains(index))
            selection->insert(index);
        else
            selection->erase(index);
    }

    bestplane = -1;
    for(i=0; i<this->planes.size(); i++)
    {
        if(!this->planes[i].RayIntersectFace(o, r, &curdist))
            continue;

        if(bestplane < 0 || curdist < bestdist)
        {
            bestdist = curdist;
            bestplane = i;
        }
    }

    if(bestplane < 0)
        return;

    this->planes[bestplane].Select(o, r, bestplane, index, ent, map);
}

void Brush::SelectTriplane(Eigen::Vector3f o, Eigen::Vector3f r, const Map& map)
{
    int i;

    for(i=0; i<this->planes.size(); i++)
    {
        if(map.selectiontype == Map::SELECT_PLANE && !this->plselection.contains(i))
            continue;

        if(!ImGui::IsKeyDown(ImGuiKey_LeftShift))
            this->planes[i].triplaneselection.clear();

        this->planes[i].SelectTriplane(o, r);
    }
}

void Brush::Draw(const Viewport& view, int index, int ent, const Map& map)
{
    int i;

    for(i=0; i<this->planes.size(); i++)
        this->planes[i].Draw(view, i, index, ent, map);
}