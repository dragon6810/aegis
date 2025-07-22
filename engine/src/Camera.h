#pragma once

#include "Matrix.h"
#include "Quaternion.h"
#include "Vector.h"

class Camera
{
public:
    static float cl_fov;

    float fov = 60.0;
    float aspect = 4.0 / 3.0;
    Vector3 pos = Vector3(0, 0, 0);
    Quaternion rot = Quaternion();

    Matrix4x4 GetView(void);
    void SetupGL(void);
};