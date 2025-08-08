#include "Plane.h"

#include "Map.h"

Plane::Plane(void)
{

}

Plane::Plane(Map& map)
{
    TextureManager::texture_t *tex;

    tex = map.GetSelectedTextureID();
    if(!tex)
        return;

    this->texname = tex->name;
}

void Plane::DrawWire(const Viewport& view, bool drawselected)
{
    int i;

    Eigen::Vector3f l[2];

    if(drawselected)
        glColor3f(1, 0, 0);
    else
        glColor3f(1, 1, 1);

    if(drawselected)
        glDisable(GL_DEPTH_TEST);

    glBegin(GL_LINES);

    for(i=0; i<this->poly.size(); i++)
    {
        l[0] = this->poly[i];
        l[1] = this->poly[(i+1)%this->poly.size()];
        glVertex3f(l[0][0], l[0][1], l[0][2]);
        glVertex3f(l[1][0], l[1][1], l[1][2]);
    }

    if(drawselected)
        glEnable(GL_DEPTH_TEST);

    glEnd();
}

void Plane::DrawShaded(const Viewport& view, bool drawselected, Map& map)
{
    int i;

    TextureManager::texture_t *tex;
    float brightness;
    Eigen::Vector2f texcoord;

    tex = map.texmanager.FindTexture(this->texname.c_str());

    brightness = (this->normal.dot(Map::light_dir) / 2.0 + 0.5) * (1.0 - Map::light_ambient) + Map::light_ambient;
    if(brightness > 1)
        brightness = 1;
    if(brightness < 0)
        brightness = 0;

    if(drawselected)
        glColor3f(brightness, brightness * 0.5, brightness * 0.5);
    else
        glColor3f(brightness, brightness, brightness);

    if(tex)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex->glid);
    }

    glBegin(GL_POLYGON);

    for(i=0; i<this->poly.size(); i++)
    {
        if(tex)
        {
            texcoord[0] = this->poly[i].dot(this->texbasis[0]) + this->texshift[0];
            texcoord[1] = this->poly[i].dot(this->texbasis[1]) + this->texshift[1];
            texcoord[0] /= (float) tex->size[0];
            texcoord[1] /= (float) tex->size[1];
            glTexCoord2f(texcoord[0], texcoord[1]);
        }
        glVertex3f(this->poly[i][0], this->poly[i][1], this->poly[i][2]);
    }

    glEnd();

    if(tex)
        glDisable(GL_TEXTURE_2D);
}

void Plane::AlignTexGrid(void)
{
    int i;

    int axis;
    float longestlen;
    bool flip;

    for(i=0; i<3; i++)
    {
        if(!i || fabsf(normal[i]) > longestlen)
        {
            axis = i;
            longestlen = fabsf(normal[i]);
            flip = normal[i] < 0;
        }
    }

    switch(axis)
    {
    case 0:
        this->texbasis[0] =  Eigen::Vector3f::UnitY();
        this->texbasis[1] =  Eigen::Vector3f::UnitZ();
        break;
    case 1:
        this->texbasis[0] = -Eigen::Vector3f::UnitX();
        this->texbasis[1] =  Eigen::Vector3f::UnitZ();
        break;
    case 2:
        this->texbasis[0] =  Eigen::Vector3f::UnitX();
        this->texbasis[1] =  Eigen::Vector3f::UnitY();
        break;
    default:
        break;
    }

    this->texshift[0] = this->texshift[1] = 0;
    
    if(flip)
        this->texbasis[0] = -this->texbasis[0];
}

void Plane::AlignTexFace(void)
{
    this->AlignTexGrid();

    this->texbasis[0] = this->texbasis[1].cross(this->normal);
    this->texbasis[1] = this->normal.cross(this->texbasis[0]);
}

bool Plane::RayIntersectFace(Eigen::Vector3f o, Eigen::Vector3f d, float* dist)
{
    int i;

    float t;
    Eigen::Vector3f edgenormal;
    Eigen::Vector3f e[2];
    Eigen::Vector3f p;

    if(this->normal.dot(d) == 0)
        return false;
    
    t = (this->normal.dot(o) - this->d) / -this->normal.dot(d);
    if(t < 0)
        return false;
    p = o + t * d;

    for(i=0; i<this->poly.size(); i++)
    {
        e[0] = this->poly[i];
        e[1] = this->poly[(i + 1) % this->poly.size()];
        edgenormal = (e[1] - e[0]).normalized().cross(this->normal);
        if(edgenormal.dot(p - e[0]) > 0)
            return false;
    }

    if(dist)
        *dist = t;

    return true;
}

void Plane::Select(Eigen::Vector3f o, Eigen::Vector3f r, int index, int brush, int ent, Map& map)
{
    std::unordered_set<int> *selection;

    selection = &map.entities[ent].brushes[brush].plselection;
    if(!selection->contains(index))
        selection->insert(index);
    else
        selection->erase(index);
}

void Plane::SelectVerts(Eigen::Vector3f o, Eigen::Vector3f r, Brush& brush, const Viewport& view)
{
    const float pixelradius = 2.0;

    int i;

    float worldradius;
    Eigen::Vector3f basis[3];
    float viewheight;

    view.GetViewBasis(basis);
    if(view.type == Viewport::FREECAM)
        viewheight = tanf(DEG2RAD(view.fov) / 2.0) * 2.0;
    else
        viewheight = view.zoom * 2.0;

    if(!ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftShift))
        this->indexselection.clear();

    for(i=0; i<this->indices.size(); i++)
    {
        worldradius = pixelradius / (float) view.canvassize[1] * viewheight;

        if(view.type == Viewport::FREECAM)
            worldradius *= (brush.points[this->indices[i]] - view.pos).dot(basis[0]);

        if(!Mathlib::RayCuboid(o, r, brush.points[this->indices[i]], worldradius).hit)
            continue;

        if(!this->indexselection.contains(i))
            this->indexselection.insert(i);
        else
            this->indexselection.erase(i);
    }
}

void Plane::Move(Eigen::Vector3f add)
{
    this->d += this->normal.dot(add);
}

void Plane::Draw(const Viewport& view, int index, int brush, int ent, Map& map, bool drawselected)
{
    int i;

    bool selected;

    switch(map.selectiontype)
    {
    case Map::SELECT_PLANE:
        selected = map.entities[ent].brushes[brush].plselection.contains(index);
        break;
    case Map::SELECT_BRUSH:
        selected = map.entities[ent].brselection.contains(brush);
        break;
    case Map::SELECT_ENTITY:
        selected = map.entselection.contains(ent);
        break;
    default:
        selected = false;
    }

    if(drawselected != selected)
        return;

    if(view.wireframe)
        this->DrawWire(view, selected);
    else
        this->DrawShaded(view, selected, map);

    if(selected && map.tool == Map::TOOL_VERTEX)
        map.entities[ent].brushes[brush].drawvertexpreview = true;
}