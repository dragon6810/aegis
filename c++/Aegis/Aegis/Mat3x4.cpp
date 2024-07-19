#include "Mat3x4.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

Mat3x4::Mat3x4()
{
    val[0][0] = 1; val[0][1] = 0; val[0][2] = 0; val[0][3] = 0;
    val[1][0] = 0; val[1][1] = 1; val[1][2] = 0; val[1][3] = 0;
    val[2][0] = 0; val[2][1] = 0; val[2][2] = 1; val[2][3] = 0;
}

Vector3 Mat3x4::operator*(Vector3 a)
{
    vec3_t v;
	v.x = a.get(0) * val[0][0] + a.get(1) * val[0][1] + a.get(2) * val[0][2] + val[0][3];
    v.y = a.get(0) * val[1][0] + a.get(1) * val[1][1] + a.get(2) * val[1][2] + val[1][3];
    v.z = a.get(0) * val[2][0] + a.get(1) * val[2][1] + a.get(2) * val[2][2] + val[2][3];
	return Vector3(v);
}

Mat3x4 Mat3x4::operator*(Mat3x4 b)
{
    Mat3x4 result;
    
    result.val[0][0] = val[0][0] * b.val[0][0] + val[0][1] * b.val[1][0] + val[0][2] * b.val[2][0];
    result.val[0][1] = val[0][0] * b.val[0][1] + val[0][1] * b.val[1][1] + val[0][2] * b.val[2][1];
    result.val[0][2] = val[0][0] * b.val[0][2] + val[0][1] * b.val[1][2] + val[0][2] * b.val[2][2];
    result.val[0][3] = val[0][0] * b.val[0][3] + val[0][1] * b.val[1][3] + val[0][2] * b.val[2][3] + val[0][3];

    result.val[1][0] = val[1][0] * b.val[0][0] + val[1][1] * b.val[1][0] + val[1][2] * b.val[2][0];
    result.val[1][1] = val[1][0] * b.val[0][1] + val[1][1] * b.val[1][1] + val[1][2] * b.val[2][1];
    result.val[1][2] = val[1][0] * b.val[0][2] + val[1][1] * b.val[1][2] + val[1][2] * b.val[2][2];
    result.val[1][3] = val[1][0] * b.val[0][3] + val[1][1] * b.val[1][3] + val[1][2] * b.val[2][3] + val[1][3];

    result.val[2][0] = val[2][0] * b.val[0][0] + val[2][1] * b.val[1][0] + val[2][2] * b.val[2][0];
    result.val[2][1] = val[2][0] * b.val[0][1] + val[2][1] * b.val[1][1] + val[2][2] * b.val[2][1];
    result.val[2][2] = val[2][0] * b.val[0][2] + val[2][1] * b.val[1][2] + val[2][2] * b.val[2][2];
    result.val[2][3] = val[2][0] * b.val[0][3] + val[2][1] * b.val[1][3] + val[2][2] * b.val[2][3] + val[2][3];

    return result;
}

Mat3x4 Mat3x4::getIdentity()
{
    Mat3x4 mat{};
    mat.val[0][0] = 1; mat.val[0][1] = 0; mat.val[0][2] = 0; mat.val[0][3] = 0;
    mat.val[1][0] = 0; mat.val[1][1] = 1; mat.val[1][2] = 0; mat.val[1][3] = 0;
    mat.val[2][0] = 0; mat.val[2][1] = 0; mat.val[2][2] = 1; mat.val[2][3] = 0;

    return mat;
}

bool Mat3x4::operator==(Mat3x4 a)
{
    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            if (!((a.val[r][c] > val[r][c] - 0.001F) && (a.val[r][c] < val[r][c] + 0.001F)))
                return false;
        }
    }

    return true;
}