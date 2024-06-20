#pragma once

#include <GL/glew.h>

#include "defs.h"

#define BEAM_NOISE_DIVISIONS 64

class BeamFX
{
public:
	static void FracNoise(float* noise, int divs);
	static void SineNoise(float* noise, int divs);

	static void DrawLineSegment(vec3_t a, vec3_t b, vec3_t color, float brightness, float width, GLuint texture, int texheight, float scrollspeed, vec3_t camerapos);
};

