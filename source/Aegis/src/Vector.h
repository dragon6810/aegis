#pragma once

#include <string>

class Vector3
{
public:
    Vector3();
    Vector3(float x, float y, float z);

    float x, y, z;
    
    std::string ToString();

    float& operator[](int i);
private:
    
};