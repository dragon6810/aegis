#include "Quaternion.h"

#include <stdio.h>
#include <string.h>

#include <math.h>

Quaternion Quaternion::AngleAxis(float theta, float axis[3])
{
    Quaternion q;
    float halfTheta = theta * 0.5F;
    float sinHalfTheta = sinf(halfTheta);

    q.q[0] = cosf(halfTheta);
    q.q[1] = axis[0] * sinHalfTheta;
    q.q[2] = axis[1] * sinHalfTheta;
    q.q[3] = axis[2] * sinHalfTheta;

    return q;
}

Mat3x4 Quaternion::toMat()
{
    Mat3x4 mat = Mat3x4();
    mat.val[0][0] = 1.0F - 2.0F * (q[2] * q[2] + q[3] * q[3]);
    mat.val[0][1] = 2.0F * (q[1] * q[2] - q[0] * q[3]);
    mat.val[0][2] = 2.0F * (q[1] * q[3] + q[0] * q[2]);
    mat.val[0][3] = 0.0F;

    mat.val[1][0] = 2.0F * (q[1] * q[2] + q[0] * q[3]);
    mat.val[1][1] = 1.0F - 2.0F * (q[1] * q[1] + q[3] * q[3]);
    mat.val[1][2] = 2.0F * (q[2] * q[3] - q[0] * q[1]);
    mat.val[1][3] = 0.0F;

    mat.val[2][0] = 2.0F * (q[1] * q[3] - q[0] * q[2]);
    mat.val[2][1] = 2.0F * (q[2] * q[3] + q[0] * q[1]);
    mat.val[2][2] = 1.0F - 2.0F * (q[1] * q[1] + q[2] * q[2]);
    mat.val[2][3] = 0.0F;

    return mat;
}

Quaternion Quaternion::operator=(Quaternion a)
{
	memcpy(&this->q, a.q, sizeof(float) * 4);
	return *this;
}

bool Quaternion::operator==(Quaternion a)
{
    return (q[0] == a.q[0]) && (q[1] == a.q[1]) && (q[2] == a.q[2]) && (q[3] == a.q[3]);
}

Quaternion Quaternion::operator*(Quaternion a)
{
    Quaternion result;

    // Extract components for readability
    float a1 = q[0], b1 = q[1], c1 = q[2], d1 = q[3];
    float a2 = a.q[0], b2 = a.q[1], c2 = a.q[2], d2 = a.q[3];

    // Compute the components of the product quaternion
    result.q[0] = a1 * a2 - b1 * b2 - c1 * c2 - d1 * d2;
    result.q[1] = a1 * b2 + b1 * a2 + c1 * d2 - d1 * c2;
    result.q[2] = a1 * c2 - b1 * d2 + c1 * a2 + d1 * b2;
    result.q[3] = a1 * d2 + b1 * c2 - c1 * b2 + d1 * a2;

    return result;
}

Quaternion Quaternion::FromAngle(float axis[3])
{
    float sy = sin(axis[2] * 0.5F);
    float cy = cos(axis[2] * 0.5F);

    float sp = sin(axis[1] * 0.5F);
    float cp = cos(axis[1] * 0.5F);

    float sr = sin(axis[0] * 0.5F);
    float cr = cos(axis[0] * 0.5F);

    Quaternion q;
    q.q[0] = sr * cp * cy - cr * sp * sy; // X component
    q.q[1] = cr * sp * cy + sr * cp * sy; // Y component
    q.q[2] = cr * cp * sy - sr * sp * cy; // Z component
    q.q[3] = cr * cp * cy + sr * sp * sy; // W component

    return q;
}