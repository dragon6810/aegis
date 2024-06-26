#pragma once

#include "defs.h"

#define M_PI 3.141592

vec3_t NormalizeVector3(vec3_t v);
float Vector3Length(vec3_t v);
vec3_t CrossProduct(vec3_t a, vec3_t b);
float DotProduct(vec3_t a, vec3_t b);

float Lerp(float a, float b, float t);