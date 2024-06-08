#include "Vector3.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define LENGTHACCESSFLAG 1
#define NORMALACCESSFLAG 2

Vector3::Vector3(float val[3])
{
	memcpy(this->val, val, sizeof(float) * 3);
	memset(this->vnormalized, 0.0F, sizeof(float) * 3);
	this->vlength = 0.0F;
	this->accessflags = 0;
}

void Vector3::set(int which, float val)
{
	this->val[which] = val;
	accessflags = 0;
}

float Vector3::get(int which)
{
	return this->val[which];
}

float Vector3::length()
{
	if (~accessflags & LENGTHACCESSFLAG)
	{
		this->vlength = sqrtf(val[0] * val[0] + val[1] * val[1] + val[2] * val[2]);
		accessflags |= LENGTHACCESSFLAG;
	}

	return this->vlength;
}

Vector3 Vector3::normalize()
{
	this->normalized();
	memcpy(this->val, this->vnormalized, sizeof(float) * 3);
	return *this;
}

Vector3 Vector3::normalized()
{
	if (~accessflags & NORMALACCESSFLAG)
	{
		this->vnormalized[0] = this->val[0] / this->length();
		this->vnormalized[1] = this->val[1] / this->length();
		this->vnormalized[2] = this->val[2] / this->length();
		accessflags |= NORMALACCESSFLAG;
	}

	return Vector3(this->vnormalized);
}

float Vector3::dot(Vector3 a, Vector3 b)
{
	return a.get(0) * b.get(0) + a.get(1) * b.get(1) + a.get(2) * b.get(2);
}

Vector3 Vector3::cross(Vector3 a, Vector3 b) 
{
	float v[3]{};
	v[0] = a.get(1) * b.get(2) - a.get(2) * b.get(1);
	v[1] = a.get(2) * b.get(0) - a.get(0) * b.get(2);
	v[2] = a.get(0) * b.get(1) - a.get(1) * b.get(0);

	return Vector3(v);
}

Vector3 Vector3::reflect(Vector3 i, Vector3 n)
{
	return i - n * 2.0 * dot(i, n);
}

Vector3 Vector3::operator+(Vector3 a)
{
	float vnew[3] =
	{
		get(0) + a.get(0),
		get(1) + a.get(1),
		get(2) + a.get(2)
	};
	return Vector3({ vnew });
}

Vector3 Vector3::operator-(Vector3 a)
{
	return *this + a * -1.0F;
}

Vector3 Vector3::operator*(float a)
{
	float vnew[3] =
	{
		get(0) * a,
		get(1) * a,
		get(2) * a
	};
	return Vector3(vnew);
}

Vector3 Vector3::operator/(float a)
{
	float vnew[3] =
	{
		get(0) / a,
		get(1) / a,
		get(2) / a
	};
	return Vector3(vnew);
}

Vector3 Vector3::operator=(Vector3 a)
{
	set(0, a.get(0));
	set(1, a.get(1));
	set(2, a.get(2));
	return *this;
}

bool Vector3::operator==(Vector3 a)
{
	return (get(0) == a.get(0)) && (get(1) == a.get(1)) && (get(2) == a.get(2));
}
