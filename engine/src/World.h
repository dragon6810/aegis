#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include <Eigen/Dense>

#include "ResourceManager.h"
#include "Wad.h"
#include "EntityBase.h"
#include "NavMesh.h"

#define NHULLS 4

#define CONTENTS_EMPTY 0
#define CONTENTS_SOLID 1

// forward declarations
class EntityCamera;
class EntityStudio;

struct plane_t
{
	Eigen::Vector3f n;
	float d;
};

struct texinfo_t
{
	ResourceManager::texture_t *tex;
	Eigen::Vector3f s, t;
	float sshift, tshift;
};

struct leaf_t
{
	Eigen::Vector3f bounds[2];
	int clipleaf;			  // index into clipleafs
	std::vector<int> surfs;   // index into surfs
};

struct portal_t
{
	std::vector<int> vertices; // index into verts
	int pl;					   // index into planes
	bool reverse;			   // reverse plane normal?
	int leaves[2];			   // index into clipleafs; [back, front]
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
	Eigen::Vector3f bounds[2];
	std::vector<int> surfs; // index into surfs
};

struct hullnode_t
{
	int pl; 		   // index into planes
	short children[2]; // if negative, bitwise inverse index into clipleafs
};

struct hullleaf_t
{
	int contents;
	std::vector<int> portals; // index into portals
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
	Eigen::Vector3f n;
	Eigen::Vector3f hit;
	Eigen::Vector3f start;
	Eigen::Vector3f end;
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
    static std::array<std::array<Eigen::Vector3f, 2>, 4> hulls;
    
    std::vector<std::shared_ptr<EntityBase>> entities;
	std::shared_ptr<EntityCamera> camera;

	int headnodes[4];

	std::vector<hullnode_t> 		   clipnodes;
	std::vector<hullleaf_t> 		   clipleafs;
	std::vector<node_t> 				   nodes;
	std::vector<leaf_t>   				   leafs;
	std::vector<portal_t>				   ports;
	std::vector<surf_t>   				   surfs;
	std::vector<Eigen::Vector3f>  		   verts;
	std::vector<plane_t> 	   			  planes;
	std::vector<texinfo_t> 	 			texinfos;
	std::unordered_map<uint32_t, model_t> models;

	std::vector<ResourceManager::texture_t*> textures;

	NavMesh navmesh;

	bool Load(std::string name);

	void Render(void);
    void Tick(void);

	traceresult_t TraceDir(int headnode, Eigen::Vector3f start, Eigen::Vector3f end);
	int GetContents(Eigen::Vector3f pos, int node);
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
    void LoadClipleaves(FILE* ptr);
	
    // Entity Loading
	void LoadEntities(FILE* ptr);
	std::shared_ptr<EntityBase> LoadEntity(const std::unordered_map<std::string, std::string>& pairs);

	// Rendering
	void RenderSurf(surf_t* surf);

	// Collision
	bool TraceDir_R(int icurnode, traceresult_t* trac, Eigen::Vector3f start, Eigen::Vector3f end, Eigen::Vector3f n);
};
