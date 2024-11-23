#include "Vector.h"

#include <c_math.h>

#include "Console.h"

Vector3::Vector3()
{
    x = y = z = 0;
}

Vector3::Vector3(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

std::string Vector3::ToString()
{
    return "(" + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + ")";
}

float Vector3::Dot(Vector3 a, Vector3 b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t)
{
    int i;

    Vector3 v;

    for(i=0; i<3; i++)
        v[i] = (b[i] - a[i]) * t + a[i];

    return v;
}

float Vector3::SqrLength()
{
    return x * x + y * y + z * z;
}

float Vector3::Length()
{
    return sqrtf(SqrLength());
}

void Vector3::Normalize()
{
    float len;

    len = 1 / Length();
    x *= len;
    y *= len;
    z *= len;
}

Vector3 Vector3::operator+(Vector3 v)
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator-(Vector3 v)
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}


float& Vector3::operator[](int i)
{
    static float err = 0;

    switch(i)
    {
        case 0:
            return x;
            break;
        case 1:
            return y;
            break;
        case 2:
            return z;
            break;
        default:
            Console::Print("Vector index out of bounds %d.\n", i);
            return err;
    }
}

Vector4::Vector4()
{
    x = y = z = w = 0;
}

Vector4::Vector4(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

std::string Vector4::ToString()
{
    return "(" + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) +  + " " + std::to_string(w) + ")";
}

float& Vector4::operator[](int i)
{
    static float err = 0;

    switch (i)
    {
    case 0:
        return x;
        break;
    case 1:
        return y;
        break;
    case 2:
        return z;
        break;
    case 3:
        return w;
        break;
    default:
        Console::Print("Vector index out of bounds %d.\n", i);
        return err;
    }
}
