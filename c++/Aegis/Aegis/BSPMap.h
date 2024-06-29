#pragma once

#include "defs.h";

#include <vector>
#include <unordered_map>
#include <memory>

#include "Skybox.h"

#include "BaseEntity.h"

class BaseEntity;

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
	char* ScanLine(char** line);
	void SetCameraPosition(vec3_t pos);
	void Draw();
	void Think(float deltatime);

	~BSPMap();

	Skybox sky;
	vec3_t camerapos = { 0.0, 0.0, 0.0 };
	vec3_t cameraforward = { 0.0, 0.0, 0.0 };
	vec3_t cameraup = { 0.0, 0.0, 0.0 };
	bspheader_t* mhdr;

	int cameraleaf;
	
	void RenderNode(short nodenum, bool renderentities);
	void RenderLeaf(short leafnum, bool renderentities);
	void RenderFace(uint16_t f);
	bool IsLeafVisible(int leaf1, int leaf2);
	vec2_t GetLightmapCoords(uint16_t f, vec3_t pos);

	std::vector<int> gltextures;
	std::vector<lightmaptexture_t> lightmaptextures;
	vec2_t maxtex[BSP_MAX_MAP_FACES];
	vec2_t mintex[BSP_MAX_MAP_FACES];

	std::vector<std::unique_ptr<BaseEntity>> entities;
	std::vector<int> EntityRenderingQueue; // Back to front rendering for potentially transparent entities
	std::unordered_map<int, std::vector<int>> leafentities;
	std::unordered_map<int, int> entityleaves;
	std::unordered_map<int, std::vector<std::unique_ptr<BaseEntity>>> facedecals;

	int LeafContents(vec3_t point);
	bool FineRaycast(vec3_t start, vec3_t end, vec3_t* intersection);

	vec3_t LightColor(vec3_t start, vec3_t end);

	void SetEntityToLeaf(int entity, int leaf);
	int GetLeafFromPoint(vec3_t p, int nodenum);
	void BoxIntersect(vec3_t bmin, vec3_t bmax, int nodenum, std::vector<int>& faces);
private:
	bool LightColorRecursive(vec3_t start, vec3_t end, int nodenum, vec3_t* color);

	int LeafContentsRecursive(vec3_t point, int iclipnode);
	bool FineRaycastRecursive(vec3_t start, vec3_t end, vec3_t* intersection, int iclipnode);
};