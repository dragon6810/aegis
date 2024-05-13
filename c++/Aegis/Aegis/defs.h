#pragma once

#define COLORSINPALETTE 256

typedef unsigned char byte_t;

typedef struct
{
	float x, y, z;
} vec3_t;

typedef struct
{
	char id[4]; // Should be IDST
	int version; // Should be 10
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
} studiomdlheader_t;

typedef struct
{
	byte_t r, g, b;
} color24_t;

typedef struct
{
	char name[64];
	int flags;
	int width;
	int height;
	int index; // Offset to pixel data
} studiomdltexture_t;

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
} studiomdlbone_t;

typedef struct
{
	int bone;
	int type;
	float start;
	float end;
	int rest;
	int index;
} studiomdlbonecontroller_t;

typedef struct
{
	int bone;
	int hitgroup;
	vec3_t bbmin;
	vec3_t bbmax;
} studiomdlhitbox_t;

typedef struct
{
	char id[4]; // Should be IDSQ
	int version; // Should be 10
	char name[64];
	int length;
} studiomdlseqheader_t;

typedef struct
{
	char label[32];
	char name[64];
	int unused1;
	int unused2;
} studiomdlseqgroup_t;

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
	int numpivots; // Unused
	int pivotindex; // Unused
	int motiontype;
	int motionbone;
	vec3_t linearmovement;
	int automoveposindex; // Unused
	int automoveangleindex; // Unused
	vec3_t bbmin;
	vec3_t bbmax;
	int numblends;
	int animindex;
	int blendtype[2];
	float blendstart[2];
	float bendend[2];
	int blendparent; // Unused
	int seqgroup;
	int entrynode;
	int exitnote;
	int nodeflags;
	int nextseq; // Unused
} studiomdlseqdescription_t;