#include "Matrix.h"

Matrix4x4::Matrix4x4()
{
    m[0][1] = m[0][2] = m[0][3] = 0;
    m[1][0] = m[1][2] = m[1][3] = 0;
    m[2][0] = m[2][1] = m[2][3] = 0;
    m[3][0] = m[3][1] = m[3][2] = 0;

    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4 a)
{
    int i, j, k;

    Matrix4x4 result;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            result.m[i][j] = 0;
            for (k = 0; k < 4; k++)
                result.m[i][j] += m[i][k] * a.m[k][j];
        }
    }
    return result;
}

float* Matrix4x4::operator[](int i)
{
	return m[i];
}