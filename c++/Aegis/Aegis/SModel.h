#pragma once

#include "mstudioload.h"
#include "Mat3x4.h"
#include "defs.h"
#include <gl/glew.h>
#include "BSPMap.h"

#define SMODEL_LIGHT_AMBIENT 192
#define SMODEL_LIGHT_DIRECT 63

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

	vec3_t camerapos{};

	Mat3x4 boneTransforms[MSTUDIOMAXBONES]{};
	vec3_t xformverts[MSTUDIOMAXMESHVERTS]{};
	vec3_t xformnorms[MSTUDIOMAXMESHVERTS]{};
	vec3_t lightvals[MSTUDIOMAXMESHVERTS]{};
	vec3_t boneup[MSTUDIOMAXBONES]{};
	vec3_t boneright[MSTUDIOMAXBONES]{};

	int bonecontrollerindices[MSTUDIOMAXBONES];
	float bonecontrollervalues[MSTUDIOMAXBONECONTROLLERS];

	GLuint* textures;

	~SModel();

	void Cleanup();

	float pos[3] = { 0.0, 0.0, 0.0 };

	int bgroup = 0;
	float adj[4];

	int curseq;
	float frame = 0.0;
	float lasttickframe = 0.0;
	float lastlasttickframe = 0.0;
	long long seqstarttime = 0;

	vec3_t lightcolor{};
	vec3_t lightdir{};
	float directlight;
	float ambientlight;

	BSPMap* map;

	void Load(const char* modelname);
	void SetPosition(float x, float y, float z);
	void startseq(int seqindex);
	void Tick();
	void SetupLighting();
	void render();
	void RenderHitboxes();
	Mat3x4 transformfrombone(int boneindex);
};