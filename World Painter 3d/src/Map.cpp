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
        projmat[1] *= -1.0f;
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

void Map::DrawWorkingBrush(const Viewport& view)
{
    const Eigen::Vector3f cornercolors[2] = { Eigen::Vector3f(1, 0, 0), Eigen::Vector3f(0, 1, 0) };
    const float dashlen = 8;

    int i, j, k;

    Eigen::Vector3i bbox[2];
    Eigen::Vector3i lengths[3];
    Eigen::Vector3i l[2];
    Eigen::Vector3i basis[3]; // forward, right, up

    if(!this->nbrushcorners)
        return;
    
    for(i=0; i<this->nbrushcorners; i++)
    {
        glBegin(GL_POINTS);
        glColor3f(cornercolors[i][0], cornercolors[i][1], cornercolors[i][2]);
        glVertex3f(this->brushcorners[i][0], this->brushcorners[i][1], this->brushcorners[i][2]);
        glEnd();
    }

    if(i < 2)
        return;
    
    glBegin(GL_POINTS);
    glColor3f(0, 1, 0);
    glVertex3f(this->brushcorners[1][0], this->brushcorners[1][1], this->brushcorners[1][2]);
    glEnd();

    bbox[0] = bbox[1] = this->brushcorners[0];
    for(i=0; i<3; i++)
    {
        if(this->brushcorners[1][i] < bbox[0][i])
            bbox[0][i] = this->brushcorners[1][i];
        if(this->brushcorners[1][i] > bbox[1][i])
            bbox[1][i] = this->brushcorners[1][i];
    }

    for(i=0; i<3; i++)
    {
        lengths[i] = Eigen::Vector3i(0, 0, 0);
        lengths[i][i] = bbox[1][i] - bbox[0][i];
    }

    glColor3f(1, 1, 0);

    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
            basis[j] = lengths[(i + j) % 3];

        for(j=0; j<4; j++)
        {
            l[0] = bbox[0];
            for(k=0; k<2; k++)
                if(j & (1 << k))
                    l[0] += basis[k + 1];
            l[1] = l[0] + basis[0];
            DrawDashedLine(l, dashlen);
        }
    }
}

void Map::DrawDashedLine(Eigen::Vector3i l[2], float dashlen)
{
    int i;

    Eigen::Vector3f dir, curpos;
    float maxlen, curlen, seglen;
    Eigen::Vector3f seg[2];

    assert(l);

    for(i=0; i<3; i++)
    {
        dir[i] = l[1][i] - l[0][i];
        curpos[i] = l[0][i];
    }
    maxlen = dir.norm();
    dir = dir.normalized();
    curlen = 0;

    glBegin(GL_LINES);

    while(curlen < maxlen)
    {
        seglen = dashlen;
        if(seglen > maxlen - curlen)
            seglen = maxlen - curlen;

        seg[0] = curpos;
        seg[1] = curpos + dir * seglen;

        for(i=0; i<2; i++)
            glVertex3f(seg[i][0], seg[i][1], seg[i][2]);

        curlen += dashlen * 2;
        curpos += dir * dashlen * 2;
    }

    glEnd();
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

    DrawWorkingBrush(view);

    glColor3f(1, 1, 1);

    glPopMatrix();
}