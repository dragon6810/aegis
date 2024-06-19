#pragma once

#include "mstudioload.h"
#include "Mat3x4.h"
#include "defs.h"
#include <gl/glew.h>

class SModel
{
public:

	typedef struct
	{
		float pos[3];
		float tex[2];
	} Vertex;

	mstudioheader_t* header;
	mstudioheader_t* texheader;
	mstudioseqheader_t* seqheader[32];

	Mat3x4 boneTransforms[MSTUDIOMAXBONES]{};
	vec3_t xformverts[MSTUDIOMAXMESHVERTS]{};
	vec3_t xformnorms[MSTUDIOMAXMESHVERTS]{};
	vec3_t lightvals[MSTUDIOMAXMESHVERTS]{};
	vec3_t boneup[MSTUDIOMAXBONES]{};
	vec3_t boneright[MSTUDIOMAXBONES]{};

	GLuint* textures;

	~SModel();

	void Cleanup();

	float pos[3] = { 0.0, 0.0, 0.0 };

	int bgroup = 0;
	float adj[4];

	int curseq;
	float frame;
	long long seqstarttime = 0;

	void Load(const char* modelname);
	void SetPosition(float x, float y, float z);
	void startseq(int seqindex);
	void render();
	Mat3x4 transformfrombone(int boneindex);
};