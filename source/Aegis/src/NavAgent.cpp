#include "NavAgent.h"

#include <GL/glew.h>

void NavAgent::RenderPath()
{
    int i;

    glBegin(GL_POINTS);
    glColor3f(1, 1, 0);
    for(i=0; i<curpath.anchors.size(); i++)
        glVertex3f(curpath.anchors[i].pos.x, curpath.anchors[i].pos.y, curpath.anchors[i].pos.z);
    glEnd();

    if(curpath.anchors.size() > 1)
    {
        glBegin(GL_LINES);
        glColor3f(0, 1, 0);
        for(i=0; i<curpath.anchors.size()-1; i++)
        {
            glVertex3f(curpath.anchors[i].pos.x, curpath.anchors[i].pos.y, curpath.anchors[i].pos.z);
            glVertex3f(curpath.anchors[i+1].pos.x, curpath.anchors[i+1].pos.y, curpath.anchors[i+1].pos.z);
        }
        glEnd();
    }

    glColor3f(1, 1, 1);
}