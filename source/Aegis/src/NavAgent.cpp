#include "NavAgent.h"

#include <GL/glew.h>

void NavAgent::RenderPath(const navpath_t& path)
{
    int i;

    glBegin(GL_POINTS);
    glColor3f(1, 1, 0);
    for(i=0; i<path.anchors.size(); i++)
        glVertex3f(path.anchors[i].pos.x, path.anchors[i].pos.y, path.anchors[i].pos.z);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0, 1, 0);
    for(i=0; i<path.anchors.size()-1; i++)
    {
        glVertex3f(path.anchors[i].pos.x, path.anchors[i].pos.y, path.anchors[i].pos.z);
        glVertex3f(path.anchors[i+1].pos.x, path.anchors[i+1].pos.y, path.anchors[i+1].pos.z);
    }
    glEnd();

    glColor3f(1, 1, 1);
}