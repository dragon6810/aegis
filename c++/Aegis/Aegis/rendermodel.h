#pragma once

#include "mstudioload.h"
#include "Mat3x4.h"
#include "defs.h"
#include <gl/glew.h>

class rendermodel
{
public:

	typedef struct
	{
		float pos[3];
		float tex[2];
	} Vertex;

	mstudioload model;
	rendermodel(unsigned int shaderProgram);
	~rendermodel();

	int bgroup = 0;
	float adj[4];

	int curseq;
	float frame;
	long long seqstarttime = 0;

	void startseq(int seqindex);
	void render(const mstudioload& model, float pos[3], GLuint* textures, const mstudioload& texmodel, mstudioseqheader_t** seqheaders);
	Mat3x4 transformfrombone(int boneindex, mstudioseqheader_t** seqheaders);
};