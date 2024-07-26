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
    Mat3x4 mat;
    
    mat.val[0][0] = 1.0F - 2.0F * (q[2] * q[2] + q[3] * q[3]);
    mat.val[0][1] = 2.0F * (q[1] * q[2] - q[3] * q[0]);
    mat.val[0][2] = 2.0F * (q[1] * q[3] + q[2] * q[0]);
    mat.val[0][3] = 0.0F;

    mat.val[1][0] = 2.0F * (q[1] * q[2] + q[3] * q[0]);
    mat.val[1][1] = 1.0F - 2.0F * (q[1] * q[1] + q[3] * q[3]);
    mat.val[1][2] = 2.0F * (q[2] * q[3] - q[1] * q[0]);
    mat.val[1][3] = 0.0F;

    mat.val[2][0] = 2.0F * (q[1] * q[3] - q[2] * q[0]);
    mat.val[2][1] = 2.0F * (q[2] * q[3] + q[1] * q[0]);
    mat.val[2][2] = 1.0F - 2.0F * (q[1] * q[1] + q[2] * q[2]);
    mat.val[2][3] = 0.0F;

    return mat;
}

Quaternion Quaternion::operator=(Quaternion a)
{
    this->q[0] = a.q[0];
    this->q[1] = a.q[1];
    this->q[2] = a.q[2];
    this->q[3] = a.q[3];
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

Quaternion Quaternion::operator*(float a)
{
    Quaternion o;
    o.q[0] = q[0] * a;
    o.q[1] = q[1] * a;
    o.q[2] = q[2] * a;
    o.q[3] = q[3] * a;

    return o;
}

Quaternion Quaternion::FromAngle(float axis[3])
{
    float axisvals[3]{};

    axisvals[0] = 1.0F;
    axisvals[1] = 0.0F;
    axisvals[2] = 0.0F;
    Quaternion x = Quaternion::AngleAxis(axis[0], axisvals);

    axisvals[0] = 0.0F;
    axisvals[1] = 1.0F;
    axisvals[2] = 0.0F;
    Quaternion y = Quaternion::AngleAxis(axis[1], axisvals);

    axisvals[0] = 0.0F;
    axisvals[1] = 0.0F;
    axisvals[2] = 1.0F;
    Quaternion z = Quaternion::AngleAxis(axis[2], axisvals);

    return z * y * x;
}

Quaternion Quaternion::FromAngle(vec3_t angles)
{
    float axis[3] = { angles.x, angles.y, angles.z };
    return FromAngle(axis);
}

Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
    float dot = q1.q[0] * q2.q[0] + q1.q[1] * q2.q[1] + q1.q[2] * q2.q[2] + q1.q[3] * q2.q[3];

    Quaternion q2Copy = q2;
    if (dot < 0.0f)
    {
        dot = -dot;
        q2Copy.q[0] = -q2.q[0];
        q2Copy.q[1] = -q2.q[1];
        q2Copy.q[2] = -q2.q[2];
        q2Copy.q[3] = -q2.q[3];
    }

    const float DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD)
    {
        Quaternion result;
        result.q[0] = q1.q[0] + t * (q2Copy.q[0] - q1.q[0]);
        result.q[1] = q1.q[1] + t * (q2Copy.q[1] - q1.q[1]);
        result.q[2] = q1.q[2] + t * (q2Copy.q[2] - q1.q[2]);
        result.q[3] = q1.q[3] + t * (q2Copy.q[3] - q1.q[3]);
        float mag = sqrt(result.q[0] * result.q[0] + result.q[1] * result.q[1] + result.q[2] * result.q[2] + result.q[3] * result.q[3]);
        result.q[0] /= mag;
        result.q[1] /= mag;
        result.q[2] /= mag;
        result.q[3] /= mag;
        return result;
    }

    float theta_0 = acos(dot);
    float theta = theta_0 * t;
    float sin_theta = sin(theta);
    float sin_theta_0 = sin(theta_0);

    float s1 = cos(theta) - dot * sin_theta / sin_theta_0;
    float s2 = sin_theta / sin_theta_0;

    Quaternion result;
    result.q[0] = (s1 * q1.q[0]) + (s2 * q2Copy.q[0]);
    result.q[1] = (s1 * q1.q[1]) + (s2 * q2Copy.q[1]);
    result.q[2] = (s1 * q1.q[2]) + (s2 * q2Copy.q[2]);
    result.q[3] = (s1 * q1.q[3]) + (s2 * q2Copy.q[3]);

    return result;
}

vec3_t Quaternion::ToEuler()
{
    float x, y, z;
    // Assuming the quaternion is normalized
    float ysqr = q[2] * q[2];

    // X (roll) rotation
    float t0 = +2.0f * (q[0] * q[1] + q[2] * q[3]);
    float t1 = +1.0f - 2.0f * (q[1] * q[1] + ysqr);
    x = atan2f(t0, t1);

    // Y (pitch) rotation
    float t2 = +2.0f * (q[0] * q[2] - q[3] * q[1]);
    t2 = t2 > 1.0f ? 1.0f : t2;
    t2 = t2 < -1.0f ? -1.0f : t2;
    y = asinf(t2);

    // Z (yaw) rotation
    float t3 = +2.0f * (q[0] * q[3] + q[1] * q[2]);
    float t4 = +1.0f - 2.0f * (ysqr + q[3] * q[3]);
    z = atan2f(t3, t4);

    return { x, y, z };
}