#include "BeamFX.h"

#include "Game.h"

#include <math.h>
#include "mathutils.h"

// https://github.com/FWGS/xash3d-fwgs/blob/d56e53a3476b32833fb7e3585e309dbe0c4561e9/ref/gl/gl_beams.c#L29

// freq2 += step * 0.1;
// Fractal noise generator, power of 2 wavelength
void BeamFX::FracNoise(float* noise, int divs)
{
	int	div2;

	div2 = divs >> 1;
	if (divs < 2) return;

	// noise is normalized to +/- scale
	noise[div2] = (noise[0] + noise[divs]) * 0.5f + divs * Game::GetGame().R_Random(-0.125, 0.125);

	if (div2 > 1)
	{
		FracNoise(&noise[div2], div2);
		FracNoise(noise, div2);
	}
}

void BeamFX::SineNoise(float* noise, int divs)
{
	float	freq = 0;
	float	step = M_PI / (float) divs * Game::GetGame().R_Random(0.75, 2.5);
	int	i;

	for (i = 0; i < divs; i++)
	{
		noise[i] = sin(freq) * Game::GetGame().R_Random(0.75, 1.0);
		freq += step;
	}
}

void BeamFX::DrawLineSegment(vec3_t a, vec3_t b, vec3_t color, float brightness, float width, GLuint texture, int texheight, float scrollspeed, vec3_t camerapos)
{
	vec3_t direction = b - a;
	vec3_t midpoint = (a + b) * (1 / 2.0);
	vec3_t tocam = camerapos - midpoint;
	vec3_t perp = NormalizeVector3(CrossProduct(tocam, direction));

	vec3_t v1 = a + perp * width;
	vec3_t v2 = b + perp * width;
	vec3_t v3 = b - perp * width;
	vec3_t v4 = a - perp * width;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glColor3f(color.x * brightness, color.y * brightness, color.z * brightness);

	float numytex = Vector3Length(direction) / texheight;
	float yoffs = Game::GetGame().Time() * scrollspeed;

	glBegin(GL_QUADS);
	glTexCoord2f(0.5, 0.0 + yoffs);
	glVertex3f(v1.x, v1.y, v1.z);
	glTexCoord2f(0.5, numytex + yoffs);
	glVertex3f(v2.x, v2.y, v2.z);
	glTexCoord2f(0.0, numytex + yoffs);
	glVertex3f(v3.x, v3.y, v3.z);
	glTexCoord2f(0.0, 0.0 + yoffs);
	glVertex3f(v4.x, v4.y, v4.z);
	glEnd();

	glColor3f(1, 1, 1);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}