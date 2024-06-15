#pragma once

#include "defs.h"

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