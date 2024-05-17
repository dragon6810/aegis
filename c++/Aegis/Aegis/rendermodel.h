#pragma once

#include "mstudioload.h"
#include "Mat3x4.h"
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

	void render(const mstudioload& model, float pos[3], GLuint* textures, const mstudioload& texmodel);
	Mat3x4 transformfrombone(float values[6]);
	Mat3x4 recursetransformfrombone(int bone, char* data);
};