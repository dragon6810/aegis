#include "Quaternion.h"

#include <stdio.h>
#include <string.h>

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