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

    bool flip;

    for(i=0; i<planes.size(); i++)
    {
        if((n - planes[i].n).squaredNorm() < epsilon * epsilon && d - planes[i].d < epsilon)
            flip = true;
        else if((n + planes[i].n).squaredNorm() < epsilon * epsilon && d + planes[i].d < epsilon)
            flip = false;

        if(flip)
            return ~i;
        else
            return i;
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

void ProcessEntity(int ent)
{

}