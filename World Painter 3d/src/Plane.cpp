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

void Plane::DrawShaded(const Viewport& view)
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

    glColor3f(brightness, brightness, brightness);

    glBegin(GL_POLYGON);

    for(i=0; i<this->poly.points.size(); i++)
    {
        glVertex3f(this->poly.points[i][0], this->poly.points[i][1], this->poly.points[i][2]);
    }

    glEnd();
}

void Plane::Draw(const Viewport& view)
{
    if(view.wireframe)
        this->DrawWire(view);
    else
        this->DrawShaded(view);
}