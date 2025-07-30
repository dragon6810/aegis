#pragma once

#include "collisionlib.h"
#include "polylib.h"

#include <math.h>

#include <Eigen/Dense>

#define DEG2RAD(x) (x * M_PI * 2.0 / 360.0)
#define RAD2DEG(x) (x * 360.0 / (M_PI * 2.0))
#define LERP(a, b, t) ((b - a) * t + a)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define SIGN(x) (x == 0 ? 0 : x < 0 ? -1 : 1)
#define TOHOMOGENOUS(v) (Eigen::Vector4f(v[0], v[1], v[2], 1.0))
#define TOHOMOGENOUSZEXT(v) (Eigen::Vector4f(v[0], v[1], v[2], 0.0))

namespace Mathlib
{
    /*
    computes a quaternion from given euler angles.
    euler angles should be in the form [roll pich yaw].
    assuming [0 0 0] is looking down the positive x-axis.
    angles are assumed to be in radians.
    */
    Eigen::Quaternionf FromEuler(Eigen::Vector3f euler);
};