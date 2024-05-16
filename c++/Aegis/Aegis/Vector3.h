#pragma once

class Vector3
{
public:
	Vector3(float val[3]);

	void set(int which, float val);
	float get(int which);

	float length();
	Vector3 normalize();
	Vector3 normalized();

	Vector3 operator+(Vector3 a);
	Vector3 operator-(Vector3 a);
	Vector3 operator*(float a);
	Vector3 operator/(float a);
	Vector3 operator=(Vector3 a);
	bool operator==(Vector3 a);
private:
	float val[3];
	float vlength;
	float vnormalized[3];
	short accessflags;
};

