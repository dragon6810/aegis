#pragma once

#include <Eigen/Dense>

#include <mathlib.h>
#include <tpklib.h>

class BrFace
{
public:
    Eigen::Vector3f n;
    float d;

    char texname[Tpklib::max_tex_name];
    Eigen::Vector3f texbasis[2];
    float texshift[2];

    Mathlib::Poly<3> poly;
};