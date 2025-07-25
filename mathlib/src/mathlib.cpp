#include <mathlib.h>

Eigen::Quaternionf Mathlib::FromEuler(Eigen::Vector3f euler)
{
    Eigen::Quaternionf q[3];

    q[0] = Eigen::Quaternionf(Eigen::AngleAxisf(euler[0], Eigen::Vector3f::UnitX()));
    q[1] = Eigen::Quaternionf(Eigen::AngleAxisf(euler[1], Eigen::Vector3f::UnitY()));
    q[2] = Eigen::Quaternionf(Eigen::AngleAxisf(euler[2], Eigen::Vector3f::UnitZ()));

    return q[2] * q[1] * q[0];
}