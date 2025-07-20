#include "Plane.h"

#include "Map.h"

void Plane::DrawWire(const Viewport& view, bool drawselected)
{
    int i;

    Eigen::Vector3f l[2];

    if(drawselected)
        glColor3f(1, 0, 0);
    else
        glColor3f(1, 1, 1);

    glBegin(GL_LINES);

    for(i=0; i<this->poly.points.size(); i++)
    {
        l[0] = this->poly.points[i];
        l[1] = this->poly.points[(i+1)%this->poly.points.size()];
        glVertex3f(l[0][0], l[0][1], l[0][2]);
        glVertex3f(l[1][0], l[1][1], l[1][2]);
    }

    glEnd();
}

void Plane::DrawShaded(const Viewport& view, bool drawselected)
{
    const Eigen::Vector3f lightdir = Eigen::Vector3f(1, 1.25, 1.5).normalized();
    const float ambient = 0.75;

    int i;

    float brightness;

    brightness = (this->normal.dot(lightdir) / 2.0 + 0.5) * (1.0 - ambient) + ambient;
    if(brightness > 1)
        brightness = 1;
    if(brightness < 0)
        brightness = 0;

    if(drawselected)
        glColor3f(brightness, brightness * 0.5, brightness * 0.5);
    else
        glColor3f(brightness, brightness, brightness);

    glBegin(GL_POLYGON);

    for(i=0; i<this->poly.points.size(); i++)
    {
        glVertex3f(this->poly.points[i][0], this->poly.points[i][1], this->poly.points[i][2]);
    }

    glEnd();
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

    for(i=0; i<this->poly.points.size(); i++)
    {
        e[0] = this->poly.points[i];
        e[1] = this->poly.points[(i + 1) % this->poly.points.size()];
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

void Plane::Draw(const Viewport& view, int index, int brush, int ent, const Map& map)
{
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

    if(view.wireframe)
        this->DrawWire(view, selected);
    else
        this->DrawShaded(view, selected);
}