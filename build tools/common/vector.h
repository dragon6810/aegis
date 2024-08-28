#ifndef __aegisvector__
#define __aegisvector__

#include "defs.h"

void VectorCopy(vec3_t dest, vec3_t src);
void VectorAdd(vec3_t dest, vec3_t a, vec3_t b);
void VectorSubtract(vec3_t dest, vec3_t a, vec3_t b);
void VectorMultiply(vec3_t dest, vec3_t a, float b);
void VectorDivide(vec3_t dest, vec3_t a, float b);
void VectorNormalize(vec3_t dest, vec3_t src);
float VectorLength(vec3_t src);
float VectorLengthSqr(vec3_t src);
float VectorDot(vec3_t a, vec3_t b);
void VectorCross(vec3_t dest, vec3_t a, vec3_t b);

#endif
