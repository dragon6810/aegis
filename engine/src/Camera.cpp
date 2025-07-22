#include "Camera.h"

Matrix4x4 Camera::GetView(void)
{
    int i;

    Matrix4x4 translate, rot;

    translate = Matrix4x4();
    for(i=0; i<3; i++)
        translate[i][3] = -this->pos[i];
    rot = this->rot.ToMatrix4().Transpose();

    return rot * translate;
}

void Camera::SetupGL(void)
{
    
}