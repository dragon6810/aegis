#pragma once
struct Quaternion
{
	float q[4];

	Quaternion AngleAxis(float theta, float axis[3]);

	Quaternion operator=(Quaternion a);
	bool operator==(Quaternion a);
	Quaternion operator*(Quaternion a);
};