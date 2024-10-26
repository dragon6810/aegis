#pragma once

#include "defs.h"

#define M_PI 3.141592

vec3_t NormalizeVector3(vec3_t v);
float Vector3Length(vec3_t v);
float Vector3Dist(vec3_t a, vec3_t b);
vec3_t CrossProduct(vec3_t a, vec3_t b);
float DotProduct(vec3_t a, vec3_t b);
vec3_t ReflectVector3(vec3_t d, vec3_t n);
vec3_t Vector3Lerp(vec3_t a, vec3_t b, float t);

vec2_t NormalizeVector2(vec2_t v);
float Vector2Length(vec2_t v);
vec2_t Vector2Lerp(vec2_t a, vec2_t b, float t);
vec2_t Vector2Bezier(vec2_t p0, vec2_t p1, vec2_t p2, float t);

float Lerp(float a, float b, float t);
float maxf(float a, float b);
float minf(float a, float b);
