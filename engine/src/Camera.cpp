#include "Camera.h"

Eigen::Matrix4f Camera::GetView(void)
{
    int i;

    Eigen::Matrix4f translate, rot;

    translate = Eigen::Matrix4f::Identity();
    translate.topRightCorner<3, 1>() = -this->pos;
    rot = Eigen::Matrix4f::Identity();
    rot.topRightCorner<3, 3>() = this->rot.toRotationMatrix().transpose();

    return rot * translate;
}

void Camera::SetupGL(void)
{
    
}