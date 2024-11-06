#include "Quaternion.h"

#include <math.h>

#include "Console.h"

Vector3 Quaternion::ToRadians(Vector3 a)
{
    return Vector3(a[0] * DEG2RAD, a[1] * DEG2RAD, a[2] * DEG2RAD);
}

Quaternion::Quaternion()
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
    Vector3 x, y, z;

    x = Vector3(1, 0, 0);
    y = Vector3(0, 1, 0);
    z = Vector3(0, 0, 1);

    return AxisAngle(z, r.y - PI / 4.0) * AxisAngle(x, r.x - PI / 2.0) * AxisAngle(y, r.z);
}

Quaternion Quaternion::AxisAngle(Vector3 a, float r)
{
    Quaternion _q;

    _q[0] = cosf(r / 2.0);
    _q[1] = sinf(r / 2.0) * cosf(a[0]);
    _q[2] = sinf(r / 2.0) * cosf(a[1]);
    _q[3] = sinf(r / 2.0) * cosf(a[2]);

    return _q;
}

Matrix4x4 Quaternion::ToMatrix4()
{
    Matrix4x4 m;

    m[0][0] = 1.0 - 2.0 * q[2] * q[2] - 2.0 * q[3] * q[3];
    m[0][1] = 2.0 * q[1] * q[2] - 2.0 * q[3] * q[0];
    m[0][2] = 2.0 * q[1] * q[3] + 2.0 * q[2] * q[0];

    m[1][0] = 2.0 * q[1] * q[2] + 2.0 * q[3] * q[0];
    m[1][1] = 1.0 - 2.0 * q[1] * q[1] - 2.0 * q[3] * q[3];
    m[1][2] = 2.0 * q[2] * q[3] - 2.0 * q[1] * q[0];

    m[2][0] = 2.0 * q[1] * q[3] - 2.0 * q[2] * q[0];
    m[2][1] = 2.0 * q[2] * q[3] + 2.0 * q[1] * q[0];
    m[2][2] = 1.0 - 2.0 * q[1] * q[1] - 2.0 * q[2] * q[2];

    return m;
}

Quaternion Quaternion::operator*(Quaternion b)
{
    Quaternion _q;

    _q[0] = q[0] * b[0] - q[1] * b[1] - q[2] * b[2] - q[3] - b[3];
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