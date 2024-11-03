#include "Vector.h"

#include "Console.h"

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