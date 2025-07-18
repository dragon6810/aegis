#include "Map.h"

#include <Eigen/Dense>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

void Map::SetupFrame(const Viewport& view)
{
    Eigen::Vector3f pos, forward, right, up;
    Eigen::Quaternionf rot;
    glm::mat4 viewmat, projmat;
    float aspect;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, view.canvassize.x(), view.canvassize.y());

    pos = view.pos;
    switch(view.type)
    {
        case Viewport::ORTHOX:
            forward = Eigen::Vector3f(-1, 0, 0);
            right = Eigen::Vector3f(0, 1, 0);
            up = Eigen::Vector3f(0, 0, 1);
            break;
        case Viewport::ORTHOY:
            forward = Eigen::Vector3f(0, -1, 0);
            right = Eigen::Vector3f(-1, 0, 0);
            up = Eigen::Vector3f(0, 0, 1);
            break;
        case Viewport::ORTHOZ:
            forward = Eigen::Vector3f(0, 0, -1);
            right = Eigen::Vector3f(1, 0, 0);
            up = Eigen::Vector3f(0, 1, 0);
            break;
        case Viewport::FREECAM:
            rot = Eigen::Quaternionf(Eigen::AngleAxisf(view.rot[0], Eigen::Vector3f::UnitX()));
            rot = Eigen::Quaternionf(Eigen::AngleAxisf(view.rot[1], Eigen::Vector3f::UnitY())) * rot;
            rot = Eigen::Quaternionf(Eigen::AngleAxisf(view.rot[2], Eigen::Vector3f::UnitZ())) * rot;
            forward = rot * Eigen::Vector3f(1, 0, 0);
            right = rot * Eigen::Vector3f(0, -1, 0);
            up = rot * Eigen::Vector3f(0, 0, 1);
            break;
        default:
            break;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(view.type == Viewport::FREECAM)
    {
        aspect = (float)view.canvassize.x() / (float)view.canvassize.y();
        projmat = glm::perspective(glm::radians(view.fov), aspect, 1.0f, max_map_size * 2.0f);
    }
    else
    {
        aspect = (float)view.canvassize.x() / (float)view.canvassize.y();
        projmat = glm::ortho(-view.zoom * aspect, view.zoom * aspect, -view.zoom, view.zoom, 1.0f, max_map_size * 2.0f);
    }
    glLoadMatrixf(&projmat[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    viewmat = glm::lookAt
    (
        glm::vec3(view.pos[0], view.pos[1], view.pos[2]),
        glm::vec3(view.pos.x() + forward.x(), view.pos.y() + forward.y(), view.pos.z() + forward.z()),
        glm::vec3(up.x(), up.y(), up.z())
    );
    glLoadMatrixf(&viewmat[0][0]);
}

void Map::Render(const Viewport& view)
{
    glPushMatrix();
    SetupFrame(view);

    glPointSize(16.0);
    glBegin(GL_POINTS);

    glColor3f(1, 1, 1);
    glVertex3f(0, 0, 0);

    glEnd();

    glPopMatrix();
}