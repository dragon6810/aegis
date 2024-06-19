#pragma once

#include "defs.h"

#include <vector>

typedef struct
{
	vec3_t o; // Origin
	vec3_t d; // Direction
	float  l; // Distance
} collisionray_t;

typedef struct
{
	bool hit; // Did the ray hit?
	vec3_t p; // Where did the ray hit?
} rayhitinfo_t;

std::vector<vec3_t> ClipPolygon(std::vector<vec3_t> poly, vec3_t n, float d);

vec3_t PlaneIntersection(vec3_t a, vec3_t b, vec3_t n, float d);

bool IsPointInPolygon(vec3_t point, const std::vector<vec3_t>& polygon);

std::vector<vec3_t> BoxFace(vec3_t bmin, vec3_t bmax, std::vector<vec3_t> face);