#pragma once

#include <Eigen/Dense>

class Camera
{
public:
    static float cl_fov;

    float fov = 60.0;
    float aspect = 4.0 / 3.0;
    Eigen::Vector3f pos = Eigen::Vector3f(0, 0, 0);
    Eigen::Quaternionf rot = Eigen::Quaternionf::Identity();

    Eigen::Matrix4f GetView(void);
    void SetupGL(void);
};