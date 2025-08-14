#include "Bsp.h"

bool verbose = false;
HullDef hulls = HullDef();
std::vector<entity_t> ents = {};
std::vector<model_t> models = {};
std::vector<plane_t> planes = {};
std::vector<node_t> nodes = {};
std::vector<leaf_t> leaves = {};
std::vector<face_t> faces = {};
std::vector<texinfo_t> texinfos = {};

int FindPlane(Eigen::Vector3f n, float d)
{
    const float epsilon = 0.1;

    int i;

    for(i=0; i<planes.size(); i++)
    {
        if((planes[i].n - n).squaredNorm() < epsilon * epsilon && fabsf(planes[i].d - d) < epsilon)
            return i;
        if((planes[i].n + n).squaredNorm() < epsilon * epsilon && fabsf(planes[i].d + d) < epsilon)
            return ~i;
    }

    planes.push_back({});
    planes[i].n = n;
    planes[i].d = d;
    return i;
}

int FindTexinfo(const char* name, Eigen::Vector3f basis[2], float shift[2])
{
    const float epsilon = 0.1;

    int i, j;

    assert(strlen(name) < Tpklib::max_tex_name);

    for(i=0; i<texinfos.size(); i++)
    {
        for(j=0; j<2; j++)
        {
            if(shift[j] - texinfos[i].shift[j] > epsilon)
                break;
            if((basis[j] - texinfos[i].basis[j]).squaredNorm() > epsilon * epsilon)
                break;
        }

        if(j < 2)
            continue;
        
        if(strcmp(name, texinfos[i].name))
            continue;

        return i;
    }

    texinfos.push_back({});
    strcpy(texinfos[i].name, name);
    for(j=0; j<2; j++)
    {
        texinfos[i].basis[j] = basis[j];
        texinfos[i].shift[j] = shift[j];
    }
    return i;
}

// lower score is better
int SplitterHeuristic(int planenum, const std::vector<int>& surfs)
{
    int i;

    Mathlib::planeside_e side;
    int counts[Mathlib::SIDE_COUNT];
    int score;

    for(i=0; i<Mathlib::SIDE_COUNT; i++)
        counts[i] = 0;

    for(i=0; i<surfs.size(); i++)
    {
        side = Mathlib::PolySide(faces[surfs[i]].poly, planes[planenum].n, planes[planenum].d);
        counts[side]++;
    }

    score = 0;
    score += abs(counts[Mathlib::SIDE_BACK] - counts[Mathlib::SIDE_FRONT]);
    score -= counts[Mathlib::SIDE_ON];
    score += counts[Mathlib::SIDE_CROSS];
    return score;
}

int FindBestSplitter(const std::vector<int>& surfs)
{
    int i;

    int score, bestscore, bestsurf;

    bestsurf = -1;
    bestscore = INT_MAX;
    for(i=0; i<surfs.size(); i++)
    {
        if(faces[surfs[i]].nodenum >= 0)
            continue;

        score = SplitterHeuristic(faces[surfs[i]].planenum, surfs);
        if(score < bestscore)
        {
            bestscore = score;
            bestsurf = i;
        }
    }

    return bestsurf;
}

void FindSurflistBB(const std::vector<int>& surfs, Eigen::Vector3f outbb[2])
{
    int i, j, k;

    for(i=0; i<surfs.size(); i++)
    {
        for(j=0; j<faces[surfs[i]].poly.size(); j++)
        {
            for(k=0; k<3; k++)
            {
                if((!i && !j) || faces[surfs[i]].poly[j][k] < outbb[0][k])
                    outbb[0][k] = faces[surfs[i]].poly[j][k];
                if((!i && !j) || faces[surfs[i]].poly[j][k] > outbb[1][k])
                    outbb[1][k] = faces[surfs[i]].poly[j][k];
            }
        }
    }
}

int nnodes, nleafs;

int BspModel_R(model_t *model, int hullnum, const std::vector<int>& surfs, int plside)
{
    int i, j, k;

    int splitsurf;
    plane_t *psplitpl;
    int inode, ileaf;
    node_t *pnode;
    leaf_t *leaf;
    Mathlib::planeside_e side;
    std::vector<int> sides[2];
    face_t face;

    splitsurf = FindBestSplitter(surfs);
    if(splitsurf < 0)
    {
        UTILS_ASSERT(side == 0 || side == 1);

        ileaf = leaves.size();
        leaves.push_back({});
        leaf = &leaves.back();

        leaf->content = plside;
        leaf->faces = surfs;
        FindSurflistBB(surfs, leaf->bb);
        
        nleafs++;
        return ~ileaf;
    }

    psplitpl = &planes[faces[surfs[splitsurf]].planenum];

    inode = nodes.size();
    nodes.push_back({});
    pnode = &nodes.back();
    pnode->planenum = faces[surfs[splitsurf]].planenum;

    FindSurflistBB(surfs, pnode->bb);

    for(i=0; i<surfs.size(); i++)
    {
        if(faces[surfs[i]].planenum != pnode->planenum)
            continue;

        pnode->faces.push_back(surfs[i]);
        faces[surfs[i]].nodenum = inode;
    }

    for(i=0; i<surfs.size(); i++)
    {
        if(faces[surfs[i]].planenum == pnode->planenum)
            continue;

        side = Mathlib::PolySide(faces[surfs[i]].poly, psplitpl->n, psplitpl->d);
        UTILS_ASSERT(side != Mathlib::SIDE_ON);
        if(side == Mathlib::SIDE_BACK)
        {
            sides[0].push_back(surfs[i]);
            continue;
        }
        if(side == Mathlib::SIDE_FRONT)
        {
            sides[1].push_back(surfs[i]);
            continue;
        }

        if(faces[surfs[i]].nodenum >= 0)
            nodes[faces[surfs[i]].nodenum].faces.push_back(faces.size());
        sides[0].push_back(surfs[i]);
        sides[1].push_back(faces.size());

        face = faces[surfs[i]];
        faces.push_back(face);
        faces[surfs[i]].poly = Mathlib::ClipPoly(face.poly, psplitpl->n, psplitpl->d, Mathlib::SIDE_BACK);
        faces.back().poly = Mathlib::ClipPoly(face.poly, psplitpl->n, psplitpl->d, Mathlib::SIDE_FRONT);
    }

    for(i=0; i<2; i++)
    {
        sides[i].reserve(sides[i].size() + pnode->faces.size());
        for(j=0; j<pnode->faces.size(); j++)
            sides[i].push_back(pnode->faces[j]);

        nodes[inode].children[i] = BspModel_R(model, hullnum, sides[i], i);
        pnode = &nodes[inode]; // children can make this a dangling pointer
    }

    nnodes++;
    return inode;
}

void BspModel(model_t *model)
{
    int h;

    uint64_t startt, endt;

    UTILS_ASSERT(model);

    printf("---- BspModel ----\n");

    startt = TIMEMS;

    nnodes = nleafs = 0;
    for(h=0; h<Bsplib::n_hulls; h++)
        model->headnodes[h] = BspModel_R(model, h, model->faces[h], -1);

    printf("%d nodes.\n", nnodes);
    printf("%d leaves.\n", nleafs);

    endt = TIMEMS;
    printf("BspModel done in %llums.\n", endt - startt);
}