#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Vector.h"
#include "ResourceManager.h"
#include "Wad.h"
#include "EntityBase.h"
#include "NavMesh.h"

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

struct leaf_t
{
	Vector3 bounds[2];
	int clipleaf;			  // index into clipleafs
	std::vector<int> surfs;   // index into surfs
};

struct portal_t
{
	std::vector<int> vertices; // index into verts
	int pl;					   // index into planes
	bool reverse;			   // reverse plane normal?
	int leaves[2];			   // index into leaves; [back, front]
};

struct surf_t
{
	std::vector<int> vertices; // index into verts; sometimes different from portal verts, often the same
    int tex;				   // index into texinfo
};

struct node_t
{
	int pl; 				// index into planes
	int children[2]; 		// index into nodes
	int leaves[2]; 			// index into leaves if >= 0
	int parent; 			// index into nodes
	Vector3 bounds[2];
	std::vector<int> surfs; // index into surfs
};

struct hullnode_t
{
	int pl; 		   // index into planes
	short children[2];
};

struct hullsurf_t
{
	std::vector<Vector3> points;
	int node; 					 // index into clipnodes
	bool flip;
};

struct model_t
{
	int renderhead; 				  // index into nodes
	int clipheads[NHULLS]; 	  		  // index into clipnodes
	std::vector<int> surfs;   		  // index into surfs
	std::vector<int> portals[NHULLS]; // index into portals
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

	int headnodes[4];

	std::vector<hullnode_t> 		   clipnodes;
	std::vector<node_t> 				   nodes;
	std::vector<leaf_t>   				   leafs;
	std::vector<portal_t>				   ports;
	std::vector<surf_t>   				   surfs;
	std::vector<hullsurf_t> 		hullsurfs[4];
	std::vector<Vector3>  				   verts;
	std::vector<plane_t> 	   			  planes;
	std::vector<texinfo_t> 	 			texinfos;
	std::unordered_map<uint32_t, model_t> models;

	std::vector<ResourceManager::texture_t*> textures;

	NavMesh navmesh;

	bool Load(std::string name);

	void Render(void);
    void Tick(void);

	traceresult_t TraceDir(int headnode, Vector3 start, Vector3 end);
	int GetContents(Vector3 pos, int node);
private:
	std::string wadpath;
	std::vector<Wad> wads;

	// Loading
	bool VerifyFile(FILE* ptr);
	bool FindLump(FILE* ptr, const char* tag, uint64_t* outloc, uint64_t* outlen);
	void LoadModels(FILE* ptr);
	void LoadPlanes(FILE* ptr);
	void LoadVerts(FILE* ptr);
	void LoadTextures(FILE* ptr);
	ResourceManager::texture_t* LoadTexture(const char* name);
	void LoadPorts(FILE* ptr);
	void LoadSurfs(FILE* ptr);
	std::vector<int> LoadFromMarkEdges(FILE *ptr, int firstmarkedge, int nmarkedges);
	void LoadLeafs(FILE* ptr);
	void LoadNodes(FILE* ptr);
	void LoadClipnodes(FILE* ptr);
    void LoadHullSurfs(FILE* ptr);

    void LoadSurfs_r(std::vector<hullsurf_t> parents, int icurnode, int ihull);
	
    // Entity Loading
	void LoadEntities(FILE* ptr);
	std::shared_ptr<EntityBase> LoadEntity(const std::unordered_map<std::string, std::string>& pairs);

	// Rendering
	void RenderSurf(surf_t* surf);

	// Collision
	bool TraceDir_R(int icurnode, traceresult_t* trac, Vector3 start, Vector3 end, Vector3 n);
};
