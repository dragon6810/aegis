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
