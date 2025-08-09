#pragma once

#include <tpklib.h>

class TexInfo
{
public:
    char name[Tpklib::max_tex_name];
    Eigen::Vector3f basis[2];
    float shift[2];
};