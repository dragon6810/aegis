#include "Write.h"

#include <stdio.h>

#include <bsplib.h>
#include <utilslib.h>

#include "Bsp.h"

Bsplib::BspFile file;

std::unordered_map<int, int> facelookup;
std::unordered_map<int, int> rnodelookup;
std::unordered_map<int, int> texinfolookup;

int FindTexinfo(int texnum)
{
    int i, j;

    int itexinfo;
    Bsplib::BspFile::texinfo_t *ftexinfo;
    texinfo_t *texinfo;

    if(texinfolookup.find(texnum) != texinfolookup.end())
        return texinfolookup[texnum];

    texinfo = &texinfo[texnum];

    itexinfo = file.texinfos.size();
    file.texinfos.push_back({});
    ftexinfo = &file.texinfos.back();

    strcpy(ftexinfo->name, texinfo->name);
    for(i=0; i<2; i++)
    {
        for(j=0; j<3; j++)
            ftexinfo->basis[i][j] = texinfo->basis[i][j];
        ftexinfo->shift[i] = texinfo->shift[i];
    }

    texinfolookup[texnum] = itexinfo;
    return itexinfo;
}

void LoadRSurf(int facenum)
{
    int isurf;
    Bsplib::BspFile::rsurf_t *fsurf;
    face_t *face;

    face = &faces[facenum];

    isurf = file.rsurfs.size();
    file.rsurfs.push_back({});
    fsurf = &file.rsurfs.back();

    facelookup[facenum] = isurf;

    fsurf->plnum = face->planenum;
    UTILS_ASSERT(rnodelookup.find(face->nodenum) != rnodelookup.end());
    fsurf->nodenum = rnodelookup[face->nodenum];
    fsurf->texinfo = FindTexinfo(face->texinfo);
    fsurf->firstmvert = file.mverts.size();
    fsurf->nverts = face->poly.size();
}

int32_t LoadRLeaf(int leafnum)
{
    int i;

    int ileaf;
    Bsplib::BspFile::rleaf_t *fleaf;
    leaf_t *leaf;

    leaf = &leaves[leafnum];

    ileaf = file.rleafs.size();
    file.rleafs.push_back({});
    fleaf = &file.rleafs.back();

    fleaf->firstmrsurf = file.mrsurfs.size();
    fleaf->nsurfs = leaf->faces.size();
    fleaf->viscluster = 0;

    file.mrsurfs.reserve(file.mrsurfs.size() + leaf->faces.size());
    for(i=0; i<leaf->faces.size(); i++)
    {
        UTILS_ASSERT(facelookup.find(leaf->faces[i]) != facelookup.end());
        file.mrsurfs.push_back(facelookup[leaf->faces[i]]);
    }

    return ileaf;
}

int32_t WriteRNode(int nodenum)
{
    int i, j;

    int inode;
    Bsplib::BspFile::rnode_t *fnode;
    node_t *node;

    if(nodenum < 0)
        return ~LoadRLeaf(~nodenum);

    node = &nodes[nodenum];

    inode = file.rnodes.size();
    file.rnodes.push_back({});
    fnode = &file.rnodes.back();

    rnodelookup[nodenum] = inode;

    fnode->plnum = node->planenum;
    fnode->firstrsurf = file.rsurfs.size();
    fnode->nsurfs = node->faces.size();
    for(i=0; i<2; i++)
        for(j=0; j<3; j++)
            fnode->bb[i][j] = node->bb[i][j];

    file.rsurfs.reserve(file.rsurfs.size() + node->faces.size());
    for(i=0; i<node->faces.size(); i++)
        LoadRSurf(node->faces[i]);

    for(i=0; i<2; i++)
        file.rnodes[inode].children[i] = WriteRNode(node->children[i]);

    return inode;
}

void WriteModel(model_t *mdl)
{
    Bsplib::BspFile::model_t *fmdl;

    file.models.push_back({});
    fmdl = &file.models.back();

    fmdl->firstrleaf = WriteRNode(mdl->headnodes[0]);
}

void MakeEntString(void)
{
    int i;
    std::map<std::string, std::string>::iterator it;
    entity_t *ent;

    file.entstring.clear();

    for(i=0, ent=ents.data(); i<ents.size(); i++, ent++)
    {
        if(ent->model)
            ent->pairs["model"] = std::to_string(ent->modelnum);
        
        file.entstring.append("{\n");

        for(it=ent->pairs.begin(); it!=ent->pairs.end(); it++)
            file.entstring.append("\"" + it->first + "\": \"" + it->second + "\"\n");

        file.entstring.append("}\n");
    }
}

void WriteInfo(void)
{
    file.info.nvisclusters = 0;
}

void Write(const char* path)
{
    int i;

    uint64_t startt, endt;

    printf("---- Write ----\n");

    startt = TIMEMS;

    file = Bsplib::BspFile();

    WriteInfo();
    file.models.reserve(models.size());
    for(i=0; i<models.size(); i++)
        WriteModel(&models[i]);
    MakeEntString();

    file.Write(path);

    endt = TIMEMS;
    printf("Write done in %llums.\n", endt - startt);
}