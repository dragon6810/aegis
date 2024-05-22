#include "Mat3x4.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

Vector3 Mat3x4::operator*(Vector3 a)
{
	float x = a.get(0) * val[0][0] + a.get(1) * val[0][1] + a.get(2) * val[0][2] + val[0][3];
	float y = a.get(0) * val[1][0] + a.get(1) * val[1][1] + a.get(2) * val[1][2] + val[1][3];
	float z = a.get(0) * val[2][0] + a.get(1) * val[2][1] + a.get(2) * val[2][2] + val[2][3];
	return Vector3(new float[3] { x, y, z });
}

Mat3x4 Mat3x4::operator*(Mat3x4 b)
{
    Mat3x4 result;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result.val[i][j] = val[i][0] * b.val[0][j] +
                val[i][1] * b.val[1][j] +
                val[i][2] * b.val[2][j] +
                (j == 3 ? val[i][3] + b.val[i][3] : 0);  // Properly handle the translation components
        }
    }
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