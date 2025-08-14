#pragma once

#include <map>
#include <vector>

#include <bsplib.h>
#include <mathlib.h>
#include <tpklib.h>

#include "HullDef.h"

typedef struct
{
    std::map<std::string, std::string> pairs;
    bool model;
    int modelnum;
} entity_t;

typedef struct
{
    int planenum;
    bool flip;
    int texinfo;
    Mathlib::Poly<3> poly;
} brface_t;

typedef struct
{
    std::vector<brface_t> faces;
    Eigen::Vector3f bb[2];
} brush_t;

typedef struct
{
    std::vector<brush_t> brushes[Bsplib::n_hulls];
    std::vector<int> faces[Bsplib::n_hulls];
    int headnodes[Bsplib::n_hulls];
} model_t;

typedef struct 
{
    Eigen::Vector3f n;
    float d;
} plane_t;

typedef struct
{
    int planenum;
    Eigen::Vector3f bb[2];
    std::vector<int> faces;
    int children[2]; // if negative, bitwise not into leaves
} node_t;

typedef struct
{
    int content;
    Eigen::Vector3f bb[2];
    std::vector<int> faces;
    std::vector<int> portals;
} leaf_t;

typedef struct
{
    int planenum;
    bool flip;
    int nodenum;
    int texinfo;
    Mathlib::Poly<3> poly;
} face_t;

typedef struct
{
    char name[Tpklib::max_tex_name];
    Eigen::Vector3f basis[2];
    float shift[2];
} texinfo_t;

extern bool verbose;
extern HullDef hulls;
extern std::vector<entity_t> ents;
extern std::vector<model_t> models;
extern std::vector<plane_t> planes;
extern std::vector<node_t> nodes;
extern std::vector<leaf_t> leaves;
extern std::vector<face_t> faces;
extern std::vector<texinfo_t> texinfos;

// if negative, flipped and bitwise inverse
int FindPlane(Eigen::Vector3f n, float d);
int FindTexinfo(const char* name, Eigen::Vector3f basis[2], float shift[2]);

void BspModel(model_t *model);