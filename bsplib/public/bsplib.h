#pragma once

#include <string>
#include <vector>

#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2

namespace Bsplib
{
    constexpr int n_hulls = 4;

    class BspFile
    {
    public:
        typedef float vec_t[3];

        typedef uint32_t mvert_t;

        typedef struct rsurf_s
        {
            int32_t plnum; // if negative, bitwise inverse and flip
            uint32_t nodenum;
            uint16_t texinfo;
            uint32_t firstmvert;
            uint8_t nverts;
        } rsurf_t;
        
        typedef uint16_t mrsurf_t;

        typedef struct rleaf_s
        {
            uint16_t firstmrsurf;
            uint16_t nsurfs;
            uint32_t viscluster;
        } rleaf_t;

        typedef struct rnode_s
        {
            int32_t plnum; // if negative, bitwise inverse and flip
            uint16_t firstrsurf;
            uint16_t nsurfs;
            vec_t bb[2];
            int32_t children[2]; // if negative, bitwise not into rleaves, else index into rnodes
        } rnode_t;

        typedef struct portal_s
        {
            int32_t plnum;
            uint32_t cnode;
            int32_t cleaves[2];
            uint32_t firstmvert;
            uint8_t nverts;
        } portal_t;

        typedef uint32_t mportal_t;

        typedef struct cleaf_s
        {
            int8_t contents;
            uint32_t firstportal;
            uint16_t nportals;
        } cleaf_t;

        typedef struct cnode_s
        {
            int32_t plnum; // if negative, bitwise inverse and flip
            uint32_t firstprt;
            uint16_t nprts;
            int32_t children[2]; // if negative, bitwise not into cleaves, else index into cnodes
        } cnode_t;

        typedef struct model_s
        {
            uint32_t rheadnode;
            uint16_t nrsurfs; // to get first rsurf, look at first rsurf of headnode.
            uint32_t firstrleaf;
            uint32_t nrleaves;
            uint32_t cheadnodes[n_hulls];
        } model_t;

        typedef struct bspinfo_s
        {
            uint32_t nvisclusters;
        } bspinfo_t;
    public:
        bspinfo_t info;
        std::vector<model_t> models;
        std::vector<cnode_t> cnodes;
        std::vector<cleaf_t> cleaves;
        std::vector<mportal_t> mportals;
        std::vector<portal_t> portals;
        std::vector<rnode_t> rnodes;
        std::vector<rleaf_t> rleafs;
        std::vector<mrsurf_t> mrsurfs;
        std::vector<rsurf_t> rsurfs;
        std::vector<mvert_t> mverts;
        std::vector<vec_t> verts;
        std::string entstring;

        void Write(const char* path);
        void Read(const char* path);
    };
}