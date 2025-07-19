#include "Plane.h"

void Plane::DrawWire(const Viewport& view)
{
    int i;

    Eigen::Vector3f l[2];

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

void Plane::Draw(const Viewport& view)
{
    if(view.wireframe)
        this->DrawWire(view);
}