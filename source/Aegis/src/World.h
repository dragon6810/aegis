#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Vector.h"
#include "EntityBase.h"
#include "EntityCamera.h"

class World
{
public:
	struct plane_t
	{
		Vector3 n;
		float d;
	};

	struct surf_t
	{
		plane_t *pl;
		bool reverse;
		std::vector<Vector3*> vertices;
		Vector3 s, t;
		float sshift, tshift;
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
	std::vector<std::shared_ptr<EntityBase>> entities;
	std::shared_ptr<EntityCamera> camera;

	std::vector<node_t>   nodes;
	std::vector<leaf_t>   leafs; // Ehhhh
	std::vector<surf_t>   surfs;
	std::vector<Vector3>  verts;
	std::vector<plane_t> planes; // You broke the 4-letter synergy, man!

	bool Load(std::string name);

	void Render();
private:
	// Entity factory, update with new entity classnames
	std::unordered_map<std::string, std::function<std::shared_ptr<EntityBase>()>> entityfactory =
	{
		{"player_camera", []() { return std::make_shared<EntityCamera>(); }},
	};

	// Loading
	bool VerifyFile(FILE* ptr);
	void LoadPlanes(FILE* ptr);
	void LoadVerts(FILE* ptr);
	void LoadSurfs(FILE* ptr);
	void LoadLeafs(FILE* ptr);
	void LoadNodes(FILE* ptr);
	
	// Entity Loading
	void LoadEntities(FILE* ptr);
	std::shared_ptr<EntityBase> LoadEntity(const std::unordered_map<std::string, std::string>& pairs);
};