#pragma once

#include <stdint.h>

#define COLORSINPALETTE 256

typedef unsigned char ubyte_t;
typedef char byte_t;

typedef enum
{
	STUDIO_NF_FLATSHADE		= 0x0001,
    STUDIO_NF_CHROME		= 0x0002,
    STUDIO_NF_FULLBRIGHT	= 0x0004,
    STUDIO_NF_NOMIPS        = 0x0008,
    STUDIO_NF_ALPHA         = 0x0010,
    STUDIO_NF_ADDITIVE      = 0x0020,
    STUDIO_NF_MASKED        = 0x0040
} mstudiotexflags_t;

typedef enum
{
	STUDIO_X =  0,
	STUDIO_Y =  1,
	STUDIO_Z =  2,
	STUDIO_XR = 3,
	STUDIO_YR = 4,
	STUDIO_ZR = 5
} mstudiomotionmodes_t;

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
	ubyte_t r;
	ubyte_t g;
	ubyte_t b;
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
	color24_t colors[COLORSINPALETTE];
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

// https://developer.valvesoftware.com/wiki/BSP_(GoldSrc)

#define BSP_LUMP_ENTITIES      0
#define BSP_LUMP_PLANES        1
#define BSP_LUMP_TEXTURES      2
#define BSP_LUMP_VERTICES      3
#define BSP_LUMP_VISIBILITY    4
#define BSP_LUMP_NODES         5
#define BSP_LUMP_TEXINFO       6
#define BSP_LUMP_FACES         7
#define BSP_LUMP_LIGHTING      8
#define BSP_LUMP_CLIPNODES     9
#define BSP_LUMP_LEAVES       10
#define BSP_LUMP_MARKSURFACES 11
#define BSP_LUMP_EDGES        12
#define BSP_LUMP_SURFEDGES    13
#define BSP_LUMP_MODELS       14
#define BSP_HEADER_LUMPS      15

#define BSP_MAX_MAP_HULLS        4

#define BSP_MAX_MAP_MODELS       400
#define BSP_MAX_MAP_BRUSHES      4096
#define BSP_MAX_MAP_ENTITIES     1024
#define BSP_MAX_MAP_ENTSTRING    (128*1024)

#define BSP_MAX_MAP_PLANES       32767
#define BSP_MAX_MAP_NODES        32767
#define BSP_MAX_MAP_CLIPNODES    32767
#define BSP_MAX_MAP_LEAFS        8192
#define BSP_MAX_MAP_VERTS        65535
#define BSP_MAX_MAP_FACES        65535
#define BSP_MAX_MAP_MARKSURFACES 65535
#define BSP_MAX_MAP_TEXINFO      8192
#define BSP_MAX_MAP_EDGES        256000
#define BSP_MAX_MAP_SURFEDGES    512000
#define BSP_MAX_MAP_TEXTURES     512
#define BSP_MAX_MAP_MIPTEX       0x200000
#define BSP_MAX_MAP_LIGHTING     0x200000
#define BSP_MAX_MAP_VISIBILITY   0x200000

#define BSP_MAX_MAP_PORTALS     65536

#define BSP_MAX_KEY     32
#define BSP_MAX_VALUE   1024

#define BSP_PLANE_X 0     // Plane is perpendicular to given axis
#define BSP_PLANE_Y 1
#define BSP_PLANE_Z 2
#define BSP_PLANE_ANYX 3  // Non-axial plane is snapped to the nearest
#define BSP_PLANE_ANYY 4
#define BSP_PLANE_ANYZ 5

#define BSP_MAXTEXTURENAME 16
#define BSP_MIPLEVELS 4

#define BSP_CONTENTS_EMPTY        -1
#define BSP_CONTENTS_SOLID        -2
#define BSP_CONTENTS_WATER        -3
#define BSP_CONTENTS_SLIME        -4
#define BSP_CONTENTS_LAVA         -5
#define BSP_CONTENTS_SKY          -6
#define BSP_CONTENTS_ORIGIN       -7
#define BSP_CONTENTS_CLIP         -8
#define BSP_CONTENTS_CURRENT_0    -9
#define BSP_CONTENTS_CURRENT_90   -10
#define BSP_CONTENTS_CURRENT_180  -11
#define BSP_CONTENTS_CURRENT_270  -12
#define BSP_CONTENTS_CURRENT_UP   -13
#define BSP_CONTENTS_CURRENT_DOWN -14
#define BSP_CONTENTS_TRANSLUCENT  -15

#define BSP_MAX_MAP_HULLS 4

typedef struct
{
	int nOffset; // File offset to data
	int nLength; // Length of data
} bsplump_t;

typedef struct
{
	int nVersion;                     // Must be 30 for a valid HL BSP file
	bsplump_t lump[BSP_HEADER_LUMPS]; // Stores the directory of lumps
} bspheader_t;

typedef struct
{
	vec3_t vNormal;   // The planes normal vector
	float fDist;      // Plane equation is: vNormal * X = fDist
	int32_t nType;    // Plane type, see #defines
} bspplane_t;

typedef struct
{
	uint32_t nMipTextures; // Number of BSPMIPTEX structures
} bsptextureheader_t;

typedef struct _BSPMIPTEX
{
	char szName[BSP_MAXTEXTURENAME];  // Name of texture
	uint32_t nWidth, nHeight;         // Extends of the texture
	uint32_t nOffsets[BSP_MIPLEVELS]; // Offsets to texture mipmaps BSPMIPTEX;
} BSPMIPTEX;

typedef struct
{
	uint32_t iPlane;            // Index into Planes lump
	short iChildren[2];         // If > 0, then indices into Nodes // otherwise bitwise inverse indices into Leafs
	short nMins[3], nMaxs[3];   // Defines bounding box
	uint16_t firstFace, nFaces; // Index and count into Faces
} bspnode_t;

typedef struct
{
	vec3_t vS;
	float fSShift;    // Texture shift in s direction
	vec3_t vT;
	float fTShift;    // Texture shift in t direction
	uint32_t iMiptex; // Index into textures array
	uint32_t nFlags;  // Texture flags
} bsptexinfo_t;

typedef struct
{
	uint16_t iPlane;          // Plane the face is parallel to
	uint16_t nPlaneSide;      // Set if different normals orientation
	uint32_t iFirstEdge;      // Index of the first surfedge
	uint16_t nEdges;          // Number of consecutive surfedges
	uint16_t iTextureInfo;    // Index of the texture info structure
	ubyte_t  nStyles[4];      // Specify lighting styles
	int      nLightmapOffset; // Offsets into the raw lightmap data; if less than zero, then a lightmap was not baked for the given face.
} bspface_t;

typedef struct
{
	int nContents;                             // Contents enumeration
	int nVisOffset;                            // Offset into the visibility lump
	short nMins[3], nMaxs[3];                  // Defines bounding box
	uint16_t iFirstMarkSurface, nMarkSurfaces; // Index and count into marksurfaces array
	ubyte_t nAmbientLevels[4];                 // Ambient sound levels
} bspleaf_t;

typedef struct
{
	uint16_t iVertex[2]; // Indices into vertex array
} bspedge_t;

typedef struct
{
	float nMins[3], nMaxs[3];              // Defines bounding box
	vec3_t vOrigin;                        // Coordinates to move the // coordinate system
	int32_t iHeadnodes[BSP_MAX_MAP_HULLS]; // Index into nodes array
	int32_t nVisLeafs;                     // ???
	int32_t iFirstFace, nFaces;            // Index and count into faces
} bspmodel_t;