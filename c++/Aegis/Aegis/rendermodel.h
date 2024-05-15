#pragma once

#include "mstudioload.h"
#include <gl/glew.h>

class rendermodel
{
public:

	typedef struct
	{
		float pos[3];
		float tex[2];
	} Vertex;

	unsigned int VAO, VBO;
	unsigned int shaderProgram;
	mstudioload model;
	rendermodel(unsigned int shaderProgram);
	~rendermodel();

	void render(const mstudioload& model, float pos[3], GLuint* textures);
};