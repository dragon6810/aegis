#include "Matrix.h"

#include <cassert>

Matrix4x4::Matrix4x4(void)
{
    m[0][1] = m[0][2] = m[0][3] = 0;
    m[1][0] = m[1][2] = m[1][3] = 0;
    m[2][0] = m[2][1] = m[2][3] = 0;
    m[3][0] = m[3][1] = m[3][2] = 0;

    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
}

Matrix4x4 const Matrix4x4::Transpose(void)
{
    int i, j;

    Matrix4x4 trans;

    for(i=0; i<4; i++)
        for(j=0; j<4; j++)
            trans[i][j] = this->m[j][i];

    return trans;
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4 a)
{
    int i, j, k;

    Matrix4x4 result;

    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            result.m[i][j] = 0;
            for (k = 0; k < 4; k++)
                result.m[i][j] += m[i][k] * a.m[k][j];
        }
    }
    return result;
}

Vector3 Matrix4x4::operator*(Vector3 v)
{
    int i;

    Vector3 n;

    for(i=0; i<3; i++)
        n[i] = v[0] * m[i][0] + v[1] * m[i][1] + v[2] * m[i][2] + m[i][3];

    return n;
}

float* Matrix4x4::operator[](int i)
{
    assert(i < 4);
	return m[i];
}

Matrix3x3::Matrix3x3(void)
{
    m[0][0] = m[0][1] = m[0][2] = 0;
    m[1][0] = m[1][1] = m[1][2] = 0;
    m[2][0] = m[2][1] = m[2][2] = 0;

    m[0][0] = m[1][1] = m[2][2] = 1;
}

Vector3 Matrix3x3::SetColumn(Vector3 v, int column)
{
    assert(column < 3);

    m[0][column] = v.x;
    m[1][column] = v.y;
    m[2][column] = v.z;

    return v;
}

Matrix3x3 const Matrix3x3::Transpose(void)
{
    int i, j;

    Matrix3x3 trans;

    for(i=0; i<3; i++)
        for(j=0; j<3; j++)
            trans[i][j] = this->m[j][i];

    return trans;
}

Matrix3x3 Matrix3x3::operator*(Matrix3x3 a)
{
    int i, j, k;

    Matrix3x3 result;

    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {
            result.m[i][j] = 0;
            for (k=0; k<3; k++)
                result.m[i][j] += m[i][k] * a.m[k][j];
        }
    }
    return result;
}

Vector3 Matrix3x3::operator*(Vector3 v)
{
    int i;

    Vector3 n;

    for(i=0; i<3; i++)
        n[i] = v[0] * m[i][0] + v[1] * m[i][1] + v[2] * m[i][2];

    return n;
}

float* Matrix3x3::operator[](int i)
{
    assert(i < 3);
	return m[i];
}