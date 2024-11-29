#include "Quaternion.h"

#include <math.h>

#include "Console.h"

Vector3 Quaternion::ToRadians(Vector3 a)
{
    return Vector3(a[0] * DEG2RAD, a[1] * DEG2RAD, a[2] * DEG2RAD);
}

Quaternion::Quaternion(void)
{
    q[1] = q[2] = q[3] = 0;
    q[0] = 1;
}

Quaternion::Quaternion(float w, float x, float y, float z)
{
    q[0] = w;
    q[1] = x;
    q[2] = y;
    q[3] = z;
}

Quaternion Quaternion::FromEuler(Vector3 r)
{
    Quaternion nq;
    float cr, sr, cp, sp, cy, sy;

    cr = cos(r[0] * 0.5);
    sr = sin(r[0] * 0.5);
    cp = cos(r[1] * 0.5);
    sp = sin(r[1] * 0.5);
    cy = cos(r[2] * 0.5);
    sy = sin(r[2] * 0.5);

    nq[0] = cr * cp * cy + sr * sp * sy;
    nq[1] = sr * cp * cy - cr * sp * sy;
    nq[2] = cr * sp * cy + sr * cp * sy;
    nq[3] = cr * cp * sy - sr * sp * cy;

    return nq;
}

Quaternion Quaternion::AxisAngle(Vector3 a, float r)
{
    Quaternion _q;

    _q[0] = cosf(r / 2.0);
    _q[1] = sinf(r / 2.0) * cosf(a[0]);
    _q[2] = sinf(r / 2.0) * cosf(a[1]);
    _q[3] = sinf(r / 2.0) * cosf(a[2]);

    _q.Normalize();

    return _q;
}

Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t)
{
    int i;

    Quaternion nq;

    float c;
    float theta, s;
    float w1, w2;

    for(i=0, c=0; i<4; i++)
        c += a[i] * b[i];

    if(c > 0.9995)
    {
        for(i=0; i<4; i++)
            nq[i] = a[i] + (b[i] - a[i]) * t;

        nq.Normalize();
        return nq;
    }

    if(c < 0)
    {
        for(i=0; i<4; i++)
            b[i] = -b[i];

        c = -c;
    }

    theta = acos(c);
    s = sqrtf(1 - c * c);

    w1 = sin((1 - t) * theta) / s;
    w2 = sin(t * theta) / s;

    for(i=0; i<4; i++)
        nq[i] = w1 * a[i] + w2 * b[i];

    return nq;
}

void Quaternion::Normalize(void)
{
    float len;

    len = sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    q[0] /= len;
    q[1] /= len;
    q[2] /= len;
    q[3] /= len;
}

Matrix4x4 Quaternion::ToMatrix4(void)
{
    Matrix4x4 m;
    float len;

    len = sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);

    m[0][0] = 1.0 - 2.0 * q[2] * q[2] - 2.0 * q[3] * q[3];
    m[0][1] =       2.0 * q[1] * q[2] - 2.0 * q[3] * q[0];
    m[0][2] =       2.0 * q[1] * q[3] + 2.0 * q[2] * q[0];

    m[1][0] =       2.0 * q[1] * q[2] + 2.0 * q[3] * q[0];
    m[1][1] = 1.0 - 2.0 * q[1] * q[1] - 2.0 * q[3] * q[3];
    m[1][2] =       2.0 * q[2] * q[3] - 2.0 * q[1] * q[0];

    m[2][0] =       2.0 * q[1] * q[3] - 2.0 * q[2] * q[0];
    m[2][1] =       2.0 * q[2] * q[3] + 2.0 * q[1] * q[0];
    m[2][2] = 1.0 - 2.0 * q[1] * q[1] - 2.0 * q[2] * q[2];

    return m;
}

Quaternion Quaternion::operator*(Quaternion b)
{
    Quaternion _q;
    float a1, a2;

    _q[0] = q[0] * b[0] - q[1] * b[1] - q[2] * b[2] - q[3] * b[3];
    _q[1] = q[0] * b[1] + q[1] * b[0] + q[2] * b[3] - q[3] * b[2];
    _q[2] = q[0] * b[2] - q[1] * b[3] + q[2] * b[0] + q[3] * b[1];
    _q[3] = q[0] * b[3] + q[1] * b[2] - q[2] * b[1] + q[3] * b[0];
    
    return _q;
}

float& Quaternion::operator[](int i)
{
    static float err = 0;

    if (i < 0 || i > 3)
    {
        Console::Print("Quaternion index out of bounds %d.\n", i);
        return err;
    }

    return q[i];
}
