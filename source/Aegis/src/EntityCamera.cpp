#include "EntityCamera.h"

#include "Console.h"
#include "Matrix.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

void EntityCamera::Init(const std::unordered_map <std::string, std::string>& pairs)
{
	pos = LoadVector3(pairs, "origin", Vector3(0, 0, 0));
	rot = LoadVector3(pairs, "angles", Vector3(0, 0, 0));
}

void EntityCamera::UpdateMouse(float x, float y)
{
    Quaternion q;
    Matrix4x4 mat;

    float planew, planeh;
    float hfov;
    Vector3 realpoint;
    Vector3 forward(0, 1, 0);

    // The dimensions of the camera plane one unit forward
    planeh = 2.0 * tan(vfov * 0.5);
    planew = planeh * aspect;

    // The direction if the camera was looking down +y
    realpoint.x =  planew * (x-0.5);
    realpoint.z = -planeh * (y-0.5);
    realpoint.y = 1;
   
    // Rotate direction by camera quaternion
    q = Quaternion::FromEuler(Quaternion::ToRadians(rot));
    mat = q.ToMatrix4();

    realpoint = mat * realpoint;
    forward = mat * forward;
    realpoint.Normalize();

    mousedir = realpoint;
}

void EntityCamera::SetUpGL()
{
    Quaternion q;
    Matrix4x4 mat;

    Vector3 forward(0, 1, 0);
    Vector3 up(0, 0, 1);

    q = Quaternion::FromEuler(Quaternion::ToRadians(rot));
    mat = q.ToMatrix4();

    forward = mat * forward;
    up = mat * up;

    glm::vec3 cameraPos(pos.x, pos.y, pos.z);
    glm::vec3 cameraTarget = cameraPos + glm::vec3(forward.x, forward.y, forward.z);
    glm::vec3 cameraUp(up.x, up.y, up.z);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 projection = glm::perspective(vfov, aspect, 0.1f, 10000.0f);
    glLoadMatrixf(&projection[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glLoadMatrixf(&view[0][0]);
}
