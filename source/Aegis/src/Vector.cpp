#include "Vector.h"

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

Vector3 Vector3::operator*(Matrix4x4 m)
{
    Vector3 v;

    v[0] = x * m[0][0] + y * m[0][1] + z * m[0][2] + m[0][3];
    v[1] = x * m[1][0] + y * m[1][1] + z * m[1][2] + m[1][3];
    v[2] = x * m[2][0] + y * m[2][1] + z * m[2][2] + m[2][3];

    return v;
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
