#include "Brush.h"

#include "Map.h"

int Brush::FindVertex(Eigen::Vector3f p)
{
    int i;

    Eigen::Vector3i pi;

    pi = p.cast<int>();
    for(i=0; i<this->points.size(); i++)
    {
        if(pi != this->points[i].cast<int>())
            continue;
        
        return i;
    }

    this->points.push_back(pi.cast<float>());
    return this->points.size() - 1;
}

struct pairhash
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // TODO: make better
        return h1 ^ (h2 << 1); 
    }
};

void Brush::DrawVertexPreview(const Map& map)
{
    int i, j;
    std::unordered_set<std::pair<int, int>, pairhash>::iterator eit;
    std::unordered_set<int>::iterator pit;
    Plane *pl;

    // i think i love sets a little too much
    std::unordered_set<std::pair<int, int>, pairhash> edges;
    std::unordered_set<int> activepoints;

    for(i=0; i<this->planes.size(); i++)
    {
        pl = &this->planes[i];
        for(j=0; j<pl->indices.size(); j++)
        {
            edges.insert(std::pair<int, int>(pl->indices[j], pl->indices[(j+1)%pl->indices.size()]));
            if(map.selectiontype == Map::SELECT_PLANE && !this->plselection.contains(i))
                continue;
            
            activepoints.insert(pl->indices[j]);
        }
    }

    glBegin(GL_LINES);
    if(this->geometryvalid)
        glColor3f(1, 1, 0);
    else
        glColor3f(0, 0, 1);
    for(eit=edges.begin(); eit!=edges.end(); eit++)
    {
        glVertex3f(this->points[std::get<0>(*eit)][0], this->points[std::get<0>(*eit)][1], this->points[std::get<0>(*eit)][2]);
        glVertex3f(this->points[std::get<1>(*eit)][0], this->points[std::get<1>(*eit)][1], this->points[std::get<1>(*eit)][2]);
    }
    glEnd();

    glPointSize(4.0);
    glBegin(GL_POINTS);
    for(pit=activepoints.begin(); pit!=activepoints.end(); pit++)
    {
        if(this->pointselection.contains(*pit))
            glColor3f(1, 0, 0);
        else
            glColor3f(1, 1, 0);
        glVertex3f(this->points[*pit][0], this->points[*pit][1], this->points[*pit][2]);
    }
    glEnd();
}

void Brush::MakeFaces(void)
{
    const float epsilon = 0.01;

    int i, j, k;
    
    std::vector<Plane> newplanes;

    for(i=0; i<this->planes.size(); i++)
    {
        this->planes[i].poly.clear();
        this->planes[i].poly = Mathlib::FromPlane(this->planes[i].normal, this->planes[i].d, Map::max_map_size);
        for(j=0; j<this->planes.size(); j++)
        {
            if(i == j)
                continue;

            this->planes[i].poly = Mathlib::ClipPoly<3>(this->planes[i].poly, this->planes[j].normal, this->planes[j].d, Mathlib::SIDE_BACK);
        }
    }

    this->points.clear();
    for(i=0; i<this->planes.size(); i++)
    {
        this->planes[i].indices.resize(this->planes[i].poly.size());
        for(j=0; j<this->planes[i].indices.size(); j++)
        {
            for(k=0; k<3; k++)
                this->planes[i].poly[j][k] = (int) (this->planes[i].poly[j][k] + epsilon * SIGN(this->planes[i].poly[j][k]));
            this->planes[i].indices[j] = FindVertex(this->planes[i].poly[j]);
        }
    }

    for(i=0; i<this->planes.size(); i++)
    {
        if(this->planes[i].poly.size() >= 3)
            newplanes.push_back(this->planes[i]);
    }

    this->planes = newplanes;
}

void Brush::AddPlane(Eigen::Vector3f n, float d, Map& map)
{
    Plane p(map);

    p.normal = n;
    p.d = d;

    this->planes.push_back(p);
    this->MakeFaces();
}

void Brush::UpdateGeometryValid(void)
{
    const float epsilon = 0.1;

    int i, j;

    std::vector<Eigen::Vector3f> normals;
    std::vector<float> distances;
    std::vector<Eigen::Vector3f> poly;
    Eigen::Vector3f n;
    float d;

    // first, check if all faces are coplanar
    for(i=0; i<this->planes.size(); i++)
    {
        if(this->planes[i].indices.size() < 3)
            continue;

        poly.resize(this->planes[i].indices.size());
        for(j=0; j<this->planes[i].indices.size(); j++)
            poly[j] = this->points[this->planes[i].indices[j]];

        n = (poly[1] - poly[0]).cross(poly[2] - poly[0]);
        if(n.squaredNorm() < epsilon) // this is okay, since it just means a dead plane was made. skip
            continue;

        n.normalize();
        d = n.dot(poly[0]);

        for(j=0; j<poly.size(); j++)
        {
            if(fabsf(n.dot(poly[j]) - d) > epsilon)
            {
                this->geometryvalid = false;
                return;
            }
        }

        normals.push_back(n);
        distances.push_back(d);
    }

    // now test convexity
    for(i=0; i<normals.size(); i++)
    {
        for(j=0; j<this->points.size(); j++)
        {
            if(normals[i].dot(this->points[j]) - distances[i] > epsilon)
            {
                this->geometryvalid = false;
                return;
            }
        }
    }

    this->geometryvalid = true;
}

void Brush::FinalizeVertexEdit(void)
{
    const float epsilon = 0.1;

    int i;
    std::unordered_set<int>::iterator it;

    std::vector<Plane> newplanes;
    std::unordered_map<int, int> newplanemapping;
    std::unordered_set<int> newplaneselection;
    Eigen::Vector3f a, b, n;
    float d;
    Plane *pl;

    this->pointselection.clear();
    if(!this->geometryvalid || !this->drawvertexpreview)
        return;

    for(i=0; i<this->planes.size(); i++)
    {
        if(this->planes[i].indices.size() < 3)
            continue;

        a = this->points[this->planes[i].indices[1]] - this->points[this->planes[i].indices[0]];
        b = this->points[this->planes[i].indices[2]] - this->points[this->planes[i].indices[0]];
        n = a.cross(b);
        n.normalize();
        if(n.norm() < epsilon)
            continue;
        d = n.dot(this->points[this->planes[i].indices[0]]);

        newplanemapping[i] = newplanes.size();
        newplanes.push_back(this->planes[i]);
        pl = &newplanes.back();
        pl->normal = n;
        pl->d = d;
    }

    this->planes = newplanes;
    this->MakeFaces();
    for(it=this->plselection.begin(); it!=this->plselection.end(); it++)
        newplaneselection.insert(newplanemapping[*it]);
    this->plselection = newplaneselection;
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

        return;
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

void Brush::SelectVerts(Eigen::Vector3f o, Eigen::Vector3f r, const Map& map, const Viewport& view)
{
    int i;
    std::unordered_set<int>::iterator it;

    for(i=0; i<this->planes.size(); i++)
    {
        if(map.selectiontype == Map::SELECT_PLANE && !this->plselection.contains(i))
            continue;

        if(!ImGui::IsKeyDown(ImGuiKey_LeftShift))
            this->planes[i].indexselection.clear();

        this->planes[i].SelectVerts(o, r, *this, view);
    }

    this->pointselection.clear();
    for(i=0; i<this->planes.size(); i++)
    {
        if(map.selectiontype == Map::SELECT_PLANE && !this->plselection.contains(i))
            continue;

        for(it=this->planes[i].indexselection.begin(); it!=this->planes[i].indexselection.end(); it++)
            this->pointselection.insert(this->planes[i].indices[*it]);
    }
}

void Brush::DeleteSelected()
{
    int i;
    std::unordered_set<int>::iterator it;

    std::vector<int> toremove;
    std::unordered_set<int> newselection;

    for(it=this->plselection.begin(); it!=this->plselection.end(); it++)
        toremove.push_back(*it);

    std::sort(toremove.begin(), toremove.end());
    for(i=toremove.size()-1; i>=0; i--)
    {
        newselection.clear();
        for(it=this->plselection.begin(); it!=this->plselection.end(); it++)
        {
            if(*it == toremove[i])
                continue;
            if(*it > toremove[i])
                newselection.insert(*it + 1);
            else
                newselection.insert(*it);
        }
        this->plselection = newselection;
        this->planes.erase(this->planes.begin() + toremove[i]);
    }

    this->MakeFaces();
}

void Brush::MoveSelected(Eigen::Vector3f add)
{
    std::unordered_set<int>::iterator it;

    for(it=this->plselection.begin(); it!=this->plselection.end(); it++)
        this->planes[*it].Move(add);
    this->MakeFaces();
}

void Brush::Move(Eigen::Vector3f add)
{
    int i;

    for(i=0; i<this->planes.size(); i++)
        this->planes[i].Move(add);
    this->MakeFaces();
}

void Brush::ApplyTextureToSelected(const char* name)
{
    std::unordered_set<int>::iterator it;

    for(it=this->plselection.begin(); it!=this->plselection.end(); it++)
        this->planes[*it].texname = name;
}

void Brush::ApplyTexture(const char* name)
{
    int i;

    for(i=0; i<this->planes.size(); i++)
        this->planes[i].texname = name;
}

void Brush::Draw(const Viewport& view, int index, int ent, Map& map, bool drawselected)
{
    int i;

    this->drawvertexpreview = false;
    for(i=0; i<this->planes.size(); i++)
        this->planes[i].Draw(view, i, index, ent, map, drawselected);

    if(!this->drawvertexpreview)
        return;

    this->DrawVertexPreview(map);
}
