#include "EntityCamera.h"

#include "Console.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <mathlib.h>

void EntityCamera::Init(const std::unordered_map <std::string, std::string>& pairs)
{
	pos = LoadVector3(pairs, "origin", Eigen::Vector3f(0, 0, 0));
	rot = LoadVector3(pairs, "angles", Eigen::Vector3f(0, 0, 0));
}

void EntityCamera::Render(void)
{
    // Nothing
}

void EntityCamera::Tick(void)
{
    // Nothing
}

void EntityCamera::UpdateMouse(float x, float y)
{
    Eigen::Quaternionf q;
    Eigen::Matrix4f mat;

    float planew, planeh;
    float hfov;
    Eigen::Vector3f realpoint;
    Eigen::Vector3f corrected;

    // The dimensions of the camera plane one unit forward
    planeh = 2.0 * tan(vfov * 0.5);
    planew = planeh * aspect;

    // The direction if the camera was looking down +y
    realpoint[1] =  planew * (x-0.5);
    realpoint[2] = -planeh * (y-0.5);
    realpoint[0] = -1;
   
    // Rotate direction by camera quaternion
    corrected[0] = rot[2];
    corrected[1] = -rot[0];
    corrected[2] = rot[1];
    q = Mathlib::FromEuler(DEG2RAD(corrected));
    mat = Eigen::Matrix4f::Identity();
    mat.topLeftCorner<3, 3>() = q.toRotationMatrix();

    realpoint = (mat * TOHOMOGENOUS(realpoint)).head<3>();
    realpoint.normalize();

    mousedir = realpoint;
}

void EntityCamera::SetUpGL()
{
    Eigen::Quaternionf q;
    Eigen::Matrix4f mat;

    Eigen::Vector3f forward(-1, 0, 0);
    Eigen::Vector3f up(0, 0, 1);
    Eigen::Vector3f corrected;
    
    corrected[0] = rot[2];
    corrected[1] = -rot[0];
    corrected[2] = rot[1];
    q = Mathlib::FromEuler(DEG2RAD(corrected));
    mat = Eigen::Matrix4f::Identity();
    mat.topLeftCorner<3, 3>() = q.toRotationMatrix();

    forward = (mat * TOHOMOGENOUS(forward)).head<3>();
    up = (mat * TOHOMOGENOUS(up)).head<3>();

    glm::vec3 cameraPos(pos[0], pos[1], pos[2]);
    glm::vec3 cameraTarget = cameraPos + glm::vec3(forward[0], forward[1], forward[2]);
    glm::vec3 cameraUp(up[0], up[1], up[2]);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 projection = glm::perspective(vfov, aspect, 0.1f, 10000.0f);
    glLoadMatrixf(&projection[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glLoadMatrixf(&view[0][0]);
}
