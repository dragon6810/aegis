#pragma once

#include <string>

#include "Matrix.h"

class Vector3
{
public:
    Vector3();
    Vector3(float x, float y, float z);

    float x, y, z;
    
    std::string ToString();
    
    Vector3 operator*(Matrix4x4 m);
    float& operator[](int i);
private:
    
};

class Vector4
{
public:
    Vector4();
    Vector4(float x, float y, float z, float w);

    float x, y, z, w;

    std::string ToString();

    float& operator[](int i);
private:

};