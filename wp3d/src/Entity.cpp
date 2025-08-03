#include "Entity.h"

#include "Map.h"

void Entity::DrawCuboid(bool selected)
{
    int i, j, k;

    Eigen::Vector3f origin, basis[3], f[4];
    float brightness;

    origin = this->GetOrigin();

    glBegin(GL_QUADS);
    for(i=0; i<3; i++)
    {
        for(j=0; j<2; j++)
        {
            basis[0] = basis[1] = basis[2] = Eigen::Vector3f::Zero();
            for(k=0; k<2; k++)
            {
                basis[k] = Eigen::Vector3f::Zero();
                basis[k][(i+k)%3] = j * 2 - 1;
            }
            basis[k] = basis[0].cross(basis[1]);

            f[0] = origin + (basis[0] - basis[1] - basis[2]) * this->default_draw_radius;
            f[1] = origin + (basis[0] + basis[1] - basis[2]) * this->default_draw_radius;
            f[2] = origin + (basis[0] + basis[1] + basis[2]) * this->default_draw_radius;
            f[3] = origin + (basis[0] - basis[1] + basis[2]) * this->default_draw_radius;

            brightness = (basis[0].dot(Map::light_dir) / 2.0 - 0.5) * (1.0 - Map::light_ambient) + Map::light_ambient;
            if(selected)
                glColor3f(brightness, 0, 0);
            else
                glColor3f(brightness, 0, brightness);

            glVertex3f(f[0][0], f[0][1], f[0][2]);
            glVertex3f(f[1][0], f[1][1], f[1][2]);
            glVertex3f(f[2][0], f[2][1], f[2][2]);
            glVertex3f(f[3][0], f[3][1], f[3][2]);
        }
    }
    glEnd();
    glColor3f(1, 1, 1);
}

Eigen::Vector3f Entity::GetOrigin(void)
{
    Eigen::Vector3f origin;

    if(this->pairs.find("origin") == this->pairs.end())
        return Eigen::Vector3f::Zero();

    sscanf(this->pairs["origin"].c_str(), "%f %f %f", &origin[0], &origin[1], &origin[2]);
    return origin;
}

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
    Mathlib::hitresult_t result;

    if(!this->brushes.size() && (this->pairs.find("classname") != this->pairs.end()) && this->pairs["classname"] != "worldspawn")
    {
        result = Mathlib::RayCuboid(o, r, this->GetOrigin(), this->default_draw_radius);
        if(result.hit)
        {
            if(dist)
                *dist = (result.pos - o).dot(r);
            return true;
        }
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
        return false;

    if(dist)
        *dist = bestdist;
    
    return true;
}

void Entity::FillDefaultPairs(Fgdlib::EntityDef* def, bool overwrite)
{
    int i;
    Fgdlib::EntityPair *pair;

    if(!def)
        return;

    for(i=0, pair=def->pairs.data(); i<def->pairs.size(); i++, pair++)
    {
        if(!overwrite && this->pairs.find(pair->keyname) != this->pairs.end())
            continue;

        switch(pair->type)
        {
        case Fgdlib::EntityPair::VALTYPE_STRING:
            this->pairs[pair->keyname] = pair->defstring;
            break;
        case Fgdlib::EntityPair::VALTYPE_INTEGER:
            this->pairs[pair->keyname] = std::to_string(pair->defint);
            break;
        case Fgdlib::EntityPair::VALTYPE_FLOAT:
            this->pairs[pair->keyname] = std::to_string(pair->deffloat);
            break;
        case Fgdlib::EntityPair::VALTYPE_INT3:
            this->pairs[pair->keyname] = 
                std::to_string(pair->defint3[0]) + " " +
                std::to_string(pair->defint3[1]) + " " +
                std::to_string(pair->defint3[2]);
            break;
        default:
            break;
        }
    }
}

Fgdlib::EntityDef* Entity::GetDef(Fgdlib::FgdFile* file)
{
    int i;

    if(!file)
        return NULL;

    if(this->pairs.find("classname") == this->pairs.end())
        return NULL;

    for(i=0; i<file->ents.size(); i++)
        if(file->ents[i].classname == this->pairs["classname"])
            break;

    if(i >= file->ents.size())
        return NULL;

    return &file->ents[i];
}

void Entity::DeleteSelected(Map& map)
{
    int i;
    std::unordered_set<int>::iterator it;

    std::vector<int> toremove;
    std::unordered_set<int> newselection;

    if(map.selectiontype == Map::SELECT_BRUSH)
    {
        for(it=this->brselection.begin(); it!=this->brselection.end(); it++)
            toremove.push_back(*it);
    }
    else
    {
        for(i=0; i<this->brushes.size(); i++)
        {
            this->brushes[i].DeleteSelected();
            if(this->brushes[i].planes.size())
                continue;
            
            toremove.push_back(i);
        }
    }

    std::sort(toremove.begin(), toremove.end());
    for(i=toremove.size()-1; i>=0; i--)
    {
        newselection.clear();
        for(it=this->brselection.begin(); it!=this->brselection.end(); it++)
        {
            if(*it == toremove[i])
                continue;
            if(*it > toremove[i])
                newselection.insert(*it + 1);
            else
                newselection.insert(*it);
        }
        this->brselection = newselection;
        this->brushes.erase(this->brushes.begin() + toremove[i]);
    }
}

void Entity::MoveSelected(Map& map, Eigen::Vector3f add)
{
    int i;
    std::unordered_set<int>::iterator it;

    printf("Entity::Selected\n");

    if(map.selectiontype == Map::SELECT_BRUSH)
    {
        for(it=this->brselection.begin(); it!=this->brselection.end(); it++)
            this->brushes[*it].Move(add);
        return;
    }

    for(i=0; i<this->brushes.size(); i++)
        this->brushes[i].MoveSelected(add);
}

void Entity::Move(Eigen::Vector3f add)
{
    int i;

    Eigen::Vector3i origin;

    printf("Entity::Move\n");

    if(this->pairs.find("origin") != this->pairs.end())
    {
        origin = (this->GetOrigin() + add).cast<int>();
        this->pairs["origin"] =
            std::to_string(origin[0]) + " " +
            std::to_string(origin[1]) + " " +
            std::to_string(origin[2]);
        return;
    }

    for(i=0; i<this->brushes.size(); i++)
        this->brushes[i].Move(add);
}

void Entity::Draw(const Viewport& view, int index, Map& map, bool drawselected)
{
    int i;

    Fgdlib::EntityDef *def;

    if(!this->brushes.size() && (this->pairs.find("classname") != this->pairs.end()) && this->pairs["classname"] != "worldspawn")
        this->DrawCuboid(map.entselection.contains(index));

    for(i=0; i<this->brushes.size(); i++)
        this->brushes[i].Draw(view, i, index, map, drawselected);
}