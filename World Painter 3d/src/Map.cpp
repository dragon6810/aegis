#include "Map.h"

#include <Eigen/Dense>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

void Map::SetupFrame(const Viewport& view)
{
    Eigen::Vector3f pos, basis[3];
    Eigen::Quaternionf rot;
    glm::mat4 viewmat, projmat;
    float aspect;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, view.canvassize.x(), view.canvassize.y());

    pos = view.pos;
    Map::GetViewBasis(view, basis);

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
        projmat = glm::ortho(-view.zoom * aspect, view.zoom * aspect, view.zoom, -view.zoom, 1.0f, max_map_size * 2.0f);
    }
    glLoadMatrixf(&projmat[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    viewmat = glm::lookAt
    (
        glm::vec3(view.pos[0], view.pos[1], view.pos[2]),
        glm::vec3(view.pos.x() + basis[0].x(), view.pos.y() + basis[0].y(), view.pos.z() + basis[0].z()),
        glm::vec3(basis[2].x(), basis[2].y(), basis[2].z())
    );
    glLoadMatrixf(&viewmat[0][0]);
}

void Map::GetViewBasis(const Viewport& view, Eigen::Vector3f outbasis[3])
{
    Eigen::Quaternionf rot;

    assert(outbasis);

    switch(view.type)
    {
        case Viewport::ORTHOX:
            outbasis[0] = Eigen::Vector3f(-1, 0, 0);
            outbasis[1] = Eigen::Vector3f(0, 1, 0);
            outbasis[2] = Eigen::Vector3f(0, 0, 1);
            break;
        case Viewport::ORTHOY:
            outbasis[0] = Eigen::Vector3f(0, -1, 0);
            outbasis[1] = Eigen::Vector3f(-1, 0, 0);
            outbasis[2] = Eigen::Vector3f(0, 0, 1);
            break;
        case Viewport::ORTHOZ:
            outbasis[0] = Eigen::Vector3f(0, 0, -1);
            outbasis[1] = Eigen::Vector3f(1, 0, 0);
            outbasis[2] = Eigen::Vector3f(0, 1, 0);
            break;
        case Viewport::FREECAM:
            rot = Eigen::Quaternionf(Eigen::AngleAxisf(view.rot[0], Eigen::Vector3f::UnitX()));
            rot = Eigen::Quaternionf(Eigen::AngleAxisf(view.rot[1], Eigen::Vector3f::UnitY())) * rot;
            rot = Eigen::Quaternionf(Eigen::AngleAxisf(view.rot[2], Eigen::Vector3f::UnitZ())) * rot;
            outbasis[0] = rot * Eigen::Vector3f(1, 0, 0);
            outbasis[1] = rot * Eigen::Vector3f(0, -1, 0);
            outbasis[2] = rot * Eigen::Vector3f(0, 0, 1);
            break;
        default:
            break;
    }
}

void Map::Click(const Viewport& view, const Eigen::Vector2f& mousepos, ImGuiMouseButton_ button)
{
    int i;

    Eigen::Vector3f basis[3], clickpos;
    int icorner;

    if(view.type == Viewport::FREECAM)
        return;

    if(button != ImGuiMouseButton_Left && button != ImGuiMouseButton_Right)
        return;
    if(button == ImGuiMouseButton_Right && nbrushcorners < 1)
        return;
    if(button == ImGuiMouseButton_Left)
        icorner = 0;
    if(button == ImGuiMouseButton_Right)
        icorner = 1;

    Map::GetViewBasis(view, basis);
    basis[1] *= view.zoom * ((float)view.canvassize.x() / (float)view.canvassize.y());
    basis[2] *= view.zoom;
    clickpos = Eigen::Vector3f(0, 0, 0);
    clickpos += basis[1] *  (mousepos[0] * 2.0 - 1.0);
    clickpos += basis[2] * -(mousepos[1] * 2.0 - 1.0);
    clickpos += view.pos;
    if(nbrushcorners < 1 || (icorner == 1 && nbrushcorners < 2))
        clickpos[view.type] = 0;
    else
        clickpos[view.type] = brushcorners[icorner][view.type];

    if(!this->nbrushcorners)
        this->nbrushcorners = 1;
    if(this->nbrushcorners == 1 && icorner == 1)
        this->nbrushcorners = 2;
    for(i=0; i<3; i++)
        this->brushcorners[icorner][i] = (int) clickpos[i];
}

void Map::Render(const Viewport& view)
{
    const float axislen = 64;

    glPushMatrix();
    SetupFrame(view);

    glPointSize(8.0);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(axislen, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, axislen, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, axislen);
    glEnd();

    if(this->nbrushcorners >= 1)
    {
        glBegin(GL_POINTS);
        glColor3f(1, 0, 0);
        glVertex3f(this->brushcorners[0][0], this->brushcorners[0][1], this->brushcorners[0][2]);
        glEnd();
    }
    if(this->nbrushcorners >= 2)
    {
        glBegin(GL_POINTS);
        glColor3f(0, 1, 0);
        glVertex3f(this->brushcorners[1][0], this->brushcorners[1][1], this->brushcorners[1][2]);
        glEnd();
    }

    glColor3f(1, 1, 1);

    glPopMatrix();
}