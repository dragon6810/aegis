#pragma once

#include <string>

class Matrix4x4;

class Vector2
{
    public:
    Vector2(void);
    Vector2(float x, float y);

    float x, y, z;
    
    std::string ToString(void);

    static float Dot(Vector2 a, Vector2 b);
    static Vector2 Lerp(Vector2 a, Vector2 b, float t);

    float SqrLength(void);
    float Length(void);
    void Normalize(void);

    Vector2 operator*(float s);
    Vector2 operator/(float s);
    Vector2 operator+(Vector2 v);
    Vector2 operator-(Vector2 v);

    float& operator[](int i);
private:
    
};

class Vector3
{
public:
    Vector3(void);
    Vector3(float x, float y, float z);

    float x, y, z;
    
    std::string ToString(void);

    static float Dot(Vector3 a, Vector3 b);
    static Vector3 Lerp(Vector3 a, Vector3 b, float t);

    float SqrLength(void);
    float Length(void);
    void Normalize(void);

    Vector3 operator*(float s);
    Vector3 operator/(float s);
    Vector3 operator+(Vector3 v);
    Vector3 operator-(Vector3 v);

    float& operator[](int i);
private:
    
};

class Vector4
{
public:
    Vector4(void);
    Vector4(float x, float y, float z, float w);

    float x, y, z, w;

    std::string ToString(void);

    float& operator[](int i);
private:

};
