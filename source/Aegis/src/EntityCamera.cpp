#include "EntityCamera.h"

#include "Console.h"
#include "Matrix.h"
#include "Quaternion.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

void EntityCamera::Init(const std::unordered_map <std::string, std::string>& pairs)
{
	pos = LoadVector3(pairs, "origin", Vector3(0, 0, 0));
	rot = LoadVector3(pairs, "angles", Vector3(0, 0, 0));

	Console::Print("Camera Position: %s.\n", pos.ToString().c_str());
	Console::Print("Camera Rotation: %s.\n", rot.ToString().c_str());
}

void EntityCamera::SetUpGL()
{
    Quaternion q;
    Matrix4x4 mat;

    Vector3 forward(0, 1, 0);
    Vector3 up(0, 0, 1);

    q = Quaternion::FromEuler(Quaternion::ToRadians(rot));
    mat = q.ToMatrix4();

    forward = forward * mat;
    up = up * mat;

    glm::vec3 cameraPos(pos.x, pos.y, pos.z);
    glm::vec3 cameraTarget = cameraPos + glm::vec3(forward.x, forward.y, forward.z);
    glm::vec3 cameraUp(up.x, up.y, up.z);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 1.0f, 10000.0f);
    glLoadMatrixf(&projection[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
    glLoadMatrixf(&view[0][0]);
}