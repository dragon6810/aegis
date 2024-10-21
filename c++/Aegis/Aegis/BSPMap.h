#pragma once

#include "defs.h"

#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include "Skybox.h"

#include "BaseEntity.h"

class BaseEntity;

#define ZOMBIE_HULL 1
#define GIANT_HULL 2
#define LITTLE_HULL 3

const static vec3_t hullbounds[8] =
{
    {0, 0, 0}, {0, 0, 0},
    {-16, -16, -36}, {16, 16, 36},
    {-32, -32, -32}, {32, 32, 32},
    {-16, -16, -18}, {16, 16, 18},
};

class BSPMap
{
public:
	typedef struct
	{
		int style[BSP_FACE_NLIGHTSTYLES];
		GLuint texture[BSP_FACE_NLIGHTSTYLES];
	} lightmaptexture_t;

	void Load(const char* filename);
	void LoadEntities();
	void ScanLine(char** line, char* out);
	void SetCameraPosition(vec3_t pos);
	void Draw();
	void Think(float deltatime);

	~BSPMap();
    
    BaseEntity* player;

	Skybox sky;
	vec3_t camerapos = { 0.0, 0.0, 0.0 };
	vec3_t cameraforward = { 0.0, 0.0, 0.0 };
	vec3_t cameraup = { 0.0, 0.0, 0.0 };
	bspheader_t* mhdr;

	int cameraleaf;
	
	void RenderLeaf(short leafnum);
	void RenderFace(uint16_t f);
	bool IsLeafVisible(int leaf1, int leaf2);
	vec2_t GetLightmapCoords(uint16_t f, vec3_t pos);

	std::vector<int> gltextures;
    std::vector<int> texwidths;
    std::vector<int> texheights;
	std::vector<lightmaptexture_t> lightmaptextures;
	vec2_t maxtex[BSP_MAX_MAP_FACES];
	vec2_t mintex[BSP_MAX_MAP_FACES];
    
	std::vector<std::unique_ptr<BaseEntity> > entities;
	std::unordered_map<int, std::vector<int> > leafentities;
	std::unordered_map<int, int> entityleaves;
	std::unordered_map<int, std::vector<std::unique_ptr<BaseEntity>>> facedecals;

	std::unordered_map<std::string, std::string> keyval;

	int LeafContents(vec3_t point);
	bool FineRaycast(vec3_t start, vec3_t end, vec3_t* intersection, vec3_t* normal, int hullnum);

	vec3_t LightColor(vec3_t start, vec3_t end);

	void SetEntityToLeaf(int entity, int leaf);
	int GetLeafFromPoint(vec3_t p, int nodenum);
	void BoxIntersect(vec3_t bmin, vec3_t bmax, int nodenum, std::vector<int>& faces);

	std::vector<int> markleaves;
	std::vector<int> markfaces;
	std::vector<int> markentities;

	std::unordered_set<int> _markleaves;
	std::unordered_set<int> _markfaces;
	std::unordered_set<int> _markentities;

	void RenderLeaves();
	void RenderLeavesRecursive(int nodenum);
	void RenderFaces();
	void RenderEntities();
private:
	bool LightColorRecursive(vec3_t start, vec3_t end, int nodenum, vec3_t* color);

	int LeafContentsRecursive(vec3_t point, int iclipnode);
	bool FineRaycastRecursive(vec3_t start, vec3_t end, vec3_t* intersection, vec3_t* normal, int iclipnode, int parent, int lastplane);
};
