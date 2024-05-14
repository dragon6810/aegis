#pragma once

#define COLORSINPALETTE 256

typedef unsigned char ubyte_t;
typedef char byte_t;

typedef struct
{
	float x, y, z;
} vec3_t;

typedef struct
{
	char id[4];             // Should be IDST
	int version;            // Should be 10
	char name[64];
	int length;
	vec3_t eyeposition;
	vec3_t min;
	vec3_t max;
	vec3_t bbmin;
	vec3_t bbmax;
	int flags;
	int numbones;
	int boneindex;
	int numbonecontrollers;
	int bonecontrollerindex;
	int numhitboxes;
	int hitboxindex;
	int numseq;
	int seqindex;
	int numseqgroups;
	int seqgroupindex;
	int numtextures;
	int textureindex;
	int texturedataindex;
	int numskinref;
	int numskinfamilies;
	int skinindex;
	int numbodyparts;
	int bodypartindex;
	int numattachments;
	int attachmentindex;
	int soundtable;
	int soundindex;
	int soundgroups;
	int soundgroupindex;
	int numtransitions;
	int transitionindex;
} mstudioheader_t;

typedef struct
{
	ubyte_t r, g, b;
} color24_t;

typedef struct
{
	char name[64];
	int flags;
	int width;
	int height;
	int index;     // Offset to pixel data
} mstudiotexture_t;

typedef struct
{
	short colorsinpalette;
	color24_t colors[COLORSINPALETTE];
	short pad;
} palette_t;

typedef struct
{
	char name[32];
	int parent;
	int flags;
	int bonecontroller[6];
	float value[6];
	float scale[6];
} mstudiobone_t;

typedef struct
{
	int bone;
	int type;
	float start;
	float end;
	int rest;
	int index;
} mstudiobonecontroller_t;

typedef struct
{
	int bone;
	int hitgroup;
	vec3_t bbmin;
	vec3_t bbmax;
} mstudiohitbox_t;

typedef struct
{
	char id[4];    // Should be IDSQ
	int version;   // Should be 10
	char name[64];
	int length;
} mstudioseqheader_t;

typedef struct
{
	char label[32];
	char name[64];
	int unused1;
	int unused2;
} mstudioseqgroup_t;

typedef struct
{
	char label[32];
	float fps;
	int flags;
	int activity;
	int actweight;
	int numevents;
	int eventindex;
	int numframes;
	int numpivots;          // Unused
	int pivotindex;         // Unused
	int motiontype;
	int motionbone;
	vec3_t linearmovement;
	int automoveposindex;   // Unused
	int automoveangleindex; // Unused
	vec3_t bbmin;
	vec3_t bbmax;
	int numblends;
	int animindex;
	int blendtype[2];
	float blendstart[2];
	float bendend[2];
	int blendparent;        // Unused
	int seqgroup;
	int entrynode;
	int exitnote;
	int nodeflags;
	int nextseq;            // Unused
} mstudioseqdescription_t;

typedef struct
{
	unsigned short offset[6];
} mstudioanimchunk_t;

typedef union
{
	struct
	{
		ubyte_t valid;
		ubyte_t total;
	} num;
	short value;
} mstudioanimvalue_t;

typedef struct
{
	int frame;
	int event;
	int type;         // Unused
	char options[64];
} mstudioanimevent_t;

typedef struct
{
	char name[32];  // Unused
	int type;       // Unused
	int bone;
	vec3_t org;
	vec3_t vectors; // Unused
} mstudioattachment_t;

/*
	int index;
	index = m_bodynum / pbodypart->base;
	index = index % pbodypart->nummodels;
	m_pmodel = (mstudiomodel_t*)((byte*)m_pstudiohdr + pbodypart->modelindex) + index;
*/
typedef struct
{
	char name[64];
	int nummodels;
	int base;
	int modelindex;
} mstudiobodypart_t;

typedef struct
{
	char name[64];        // Also the name of the SMD file
	int type;             // Unused
	float boundingradius; // Unused
	int nummesh;
	int meshindex;
	int numverts;
	int vertinfoindex;
	int vertindex;
	int numnorms;
	int norminfoindex;
	int normindex;
	int numgroups;        // Unused
	int groupindex;       // Unused
} mstudiomodel_t;

typedef struct
{
	int numtris;
	int triindex;
	int skinref;
	int numnorms;  // Unused
	int normindex; // Unused
} mstudiomesh_t;

typedef struct
{
	short vertindex;
	short normindex;
	short s;
	short t;
} mstudiotrivert_t;