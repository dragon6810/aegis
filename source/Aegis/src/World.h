#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Vector.h"
#include "ResourceManager.h"
#include "Wad.h"
#include "EntityBase.h"
#include "Nav.h"

#define NHULLS 4

#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2

// forward declarations
class EntityCamera;
class EntityStudio;

struct plane_t
{
	Vector3 n;
	float d;
};

struct texinfo_t
{
	ResourceManager::texture_t *tex;
	Vector3 s, t;
	float sshift, tshift;
};

struct surf_t
{
	plane_t *pl;
	int ipl;
    bool reverse;
	std::vector<Vector3*> vertices;
    std::vector<int> ivertices;
    texinfo_t *tex;
};

struct leaf_t
{
	int contents;
	Vector3 min, max;
	std::vector<surf_t*> surfs;
};

struct node_t
{
	plane_t *pl;
	node_t *children[2];
	leaf_t *leaves[2];
	node_t *parent;
	Vector3 min, max;
	std::vector<surf_t*> surfs;
};

struct hullnode_t
{
	plane_t* pl;
	short children[2];
};

struct traceresult_t
{
	bool didhit;
	Vector3 n;
	Vector3 hit;
	Vector3 start;
	Vector3 end;
};

class World
{
private:
	enum bsplumps_e
	{
		LUMP_ENTITIES=      0,
		LUMP_PLANES=        1,
		LUMP_TEXTURES=      2,
		LUMP_VERTICES=      3,
		LUMP_VISIBILITY=    4,
		LUMP_NODES=         5,
		LUMP_TEXINFO=       6,
		LUMP_FACES=         7,
		LUMP_LIGHTING=      8,
		LUMP_CLIPNODES=     9,
		LUMP_LEAVES=       10,
		LUMP_MARKSURFACES= 11,
		LUMP_EDGES=        12,
		LUMP_SURFEDGES=    13,
		LUMP_MODELS=       14,
	};
public:
    static std::array<std::array<Vector3, 2>, 4> hulls;
    
    std::vector<std::shared_ptr<EntityBase>> entities;
	std::shared_ptr<EntityCamera> camera;

	std::vector<hullnode_t> clipnodes;
	std::vector<node_t> 		nodes;
	std::vector<leaf_t>   		leafs; // Ehhhh
	std::vector<surf_t>   		surfs;
	std::vector<Vector3>  		verts;
	std::vector<plane_t> 	   planes; // You broke the 4-letter synergy, man!
	std::vector<texinfo_t> 	 texinfos;

	std::vector<ResourceManager::texture_t*> textures;

	Nav nav;

	bool Load(std::string name);

	void Render(void);
    void Tick(void);

	traceresult_t TraceDir(int headnode, Vector3 start, Vector3 end);
private:
	std::string wadpath;
	std::vector<Wad> wads;

	// Loading
	bool VerifyFile(FILE* ptr);
	void LoadPlanes(FILE* ptr);
	void LoadVerts(FILE* ptr);
	void LoadTextures(FILE* ptr);
	ResourceManager::texture_t* LoadTexture(int index, FILE* ptr);
	void LoadSurfs(FILE* ptr);
	void LoadLeafs(FILE* ptr);
	void LoadNodes(FILE* ptr);
	void LoadClipnodes(FILE* ptr);
	
	// Entity Loading
	void LoadEntities(FILE* ptr);
	std::shared_ptr<EntityBase> LoadEntity(const std::unordered_map<std::string, std::string>& pairs);

	// Rendering
	void RenderSurf(surf_t* surf);

	// Collision
	bool TraceDir_R(int icurnode, traceresult_t* trac, Vector3 start, Vector3 end, Vector3 n);
};
