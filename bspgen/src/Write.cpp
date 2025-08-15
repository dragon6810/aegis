#include "Write.h"

#include <stdio.h>

#include <bsplib.h>
#include <utilslib.h>

#include "Bsp.h"

Bsplib::BspFile file;

std::unordered_map<int, int> facelookup;
std::unordered_map<int, int> rnodelookup;
std::unordered_map<int, int> texinfolookup;
std::unordered_map<int, int> cnodelookup;
std::unordered_map<int, int> prtlookup;
std::unordered_map<int, int> pllookup; // planes are leftover from culled faces, so not all are included in file

int FindPlane(int plnum)
{
    const float epsilon = 0.01;

    int i;

    plane_t *pl;
    Bsplib::BspFile::plane_t *newpl;

    if(pllookup.find(plnum) != pllookup.end())
        return pllookup[plnum];

    pl = &planes[plnum];
    file.planes.push_back({});
    newpl = &file.planes.back();

    newpl->axis = 4;
    for(i=0; i<3; i++)
    {
        newpl->n.v[i] = pl->n[i];
        if(fabsf(pl->n[i]) > 1 - epsilon)
            newpl->axis = i;
    }
    newpl->d = pl->d;

    pllookup[plnum] = file.planes.size() - 1;
    return file.planes.size() - 1;
}

int FindVert(Eigen::Vector3f v, float epsilon=0.1)
{
    int i, j;

    Bsplib::BspFile::vec_t newv;

    for(i=0; i<file.verts.size(); i++)
    {
        for(j=0; j<3; j++)
            if(fabsf(file.verts[i].v[j] - v[j]) > epsilon)
                break;
        if(j >= 3)
            return i;
    }

    for(i=0; i<3; i++)
        newv.v[i] = v[i];
    file.verts.push_back(newv);
    return file.verts.size() - 1;
}

void WritePortal(int prtnum)
{
    int i;

    int iprt;
    Bsplib::BspFile::portal_s *fprt;
    portal_t *prt;

    prt = &portals[prtnum];

    iprt = file.portals.size();
    file.portals.push_back({});
    fprt = &file.portals.back();

    prtlookup[prtnum] = iprt;

    fprt->plnum = FindPlane(prt->planenum);
    UTILS_ASSERT(cnodelookup.find(prt->nodenum) != cnodelookup.end());
    fprt->cnode = cnodelookup[prt->nodenum];
    fprt->firstmvert = file.mverts.size();
    fprt->nverts = prt->poly.size();

    file.mverts.reserve(file.mverts.size() + prt->poly.size());
    for(i=0; i<prt->poly.size(); i++)
        file.mverts.push_back(FindVert(prt->poly[i]));
}

int32_t WriteCLeaf(int leafnum)
{
    int i;

    int ileaf;
    Bsplib::BspFile::cleaf_t *fleaf;
    leaf_t *leaf;

    leaf = &leaves[leafnum];

    ileaf = file.cleaves.size();
    file.cleaves.push_back({});
    fleaf = &file.cleaves.back();

    fleaf->contents = leaf->contents;
    fleaf->firstmportal = file.mportals.size();
    fleaf->nportals = leaf->portals.size();

    file.mportals.reserve(file.mportals.size() + leaf->portals.size());
    for(i=0; i<leaf->portals.size(); i++)
    {
        UTILS_ASSERT(prtlookup.find(leaf->portals[i]) != prtlookup.end());
        file.mportals.push_back(prtlookup[leaf->portals[i]]);
    }

    return ileaf;
}

int32_t WriteCNode(int nodenum)
{
    int i, j;

    int inode;
    Bsplib::BspFile::cnode_t *fnode;
    node_t *node;

    if(nodenum < 0)
        return ~WriteCLeaf(~nodenum);

    node = &nodes[nodenum];

    inode = file.cnodes.size();
    file.cnodes.push_back({});
    fnode = &file.cnodes.back();

    cnodelookup[nodenum] = inode;

    fnode->plnum = FindPlane(node->planenum);

    file.portals.reserve(file.portals.size() + node->portals.size());
    for(i=0; i<node->portals.size(); i++)
        WritePortal(node->portals[i]);

    for(i=0; i<2; i++)
        file.cnodes[inode].children[i] = WriteCNode(node->children[i]);

    return inode;
}

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
            ftexinfo->basis[i].v[j] = texinfo->basis[i][j];
        ftexinfo->shift[i] = texinfo->shift[i];
    }

    texinfolookup[texnum] = itexinfo;
    return itexinfo;
}

void LoadRSurf(int facenum)
{
    int i;

    int isurf;
    Bsplib::BspFile::rsurf_t *fsurf;
    face_t *face;

    face = &faces[facenum];

    isurf = file.rsurfs.size();
    file.rsurfs.push_back({});
    fsurf = &file.rsurfs.back();

    facelookup[facenum] = isurf;

    fsurf->plnum = FindPlane(face->planenum);
    UTILS_ASSERT(rnodelookup.find(face->nodenum) != rnodelookup.end());
    fsurf->nodenum = rnodelookup[face->nodenum];
    fsurf->texinfo = FindTexinfo(face->texinfo);
    fsurf->firstmvert = file.mverts.size();
    fsurf->nverts = face->poly.size();

    file.mverts.reserve(file.mverts.size() + face->poly.size());
    for(i=0; i<face->poly.size(); i++)
        file.mverts.push_back(FindVert(face->poly[i]));
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

    fnode->plnum = FindPlane(node->planenum);
    fnode->firstrsurf = file.rsurfs.size();
    fnode->nsurfs = node->faces.size();
    for(i=0; i<2; i++)
        for(j=0; j<3; j++)
            fnode->bb[i].v[j] = node->bb[i][j];

    file.rsurfs.reserve(file.rsurfs.size() + node->faces.size());
    for(i=0; i<node->faces.size(); i++)
        LoadRSurf(node->faces[i]);

    for(i=0; i<2; i++)
        file.rnodes[inode].children[i] = WriteRNode(node->children[i]);

    return inode;
}

void WriteModel(model_t *mdl)
{
    int h;

    Bsplib::BspFile::model_t *fmdl;

    file.models.push_back({});
    fmdl = &file.models.back();

    fmdl->rheadnode = WriteRNode(mdl->headnodes[0]);
    for(h=0; h<Bsplib::n_hulls; h++)
        fmdl->cheadnodes[h] = WriteCNode(mdl->headnodes[h]);
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