#include "BspBuilder.h"

#include <maplib.h>
#include <utilslib.h>

void BspBuilder::ExpandHulls(void)
{
    int i;

    uint64_t startt, endt;

    startt = TIMEMS;

    for(i=0; i<this->ents.size(); i++)
        this->ents[i].ExpandBrushes(this->hulldef);

    endt = TIMEMS;
    printf("hull brush expansion done in %llums.\n", endt - startt);
}

void BspBuilder::CullInterior(void)
{
    int i, h;

    uint64_t startt, endt;
    int hullcounts[Bsplib::n_hulls];

    startt = TIMEMS;

    for(i=0; i<this->ents.size(); i++)
        this->ents[i].CullInterior();

    endt = TIMEMS;
    printf("interior face culling done in %llums.\n", endt - startt);

    if(verbose)
    {
        for(h=0; h<Bsplib::n_hulls; h++)
        {
            if(h < Bsplib::n_hulls - 1)
                printf("|- hull %d\n", h);
            else
                printf("`- hull %d\n", h);

            hullcounts[h] = 0;
            for(i=0; i<this->ents.size(); i++)
                hullcounts[h] += this->ents[i].bsp[h].faces.size();
            
            if(h < Bsplib::n_hulls - 1)
                printf("|");
            else
                printf(" ");

            printf("  `- %d faces\n", hullcounts[h]);
        }
    }
}

void BspBuilder::WriteCSGFaces(void)
{
    int h, e, f, v;

    std::string filename;
    FILE *ptr;
    std::vector<BspFace> *curgeo;
    std::vector<Eigen::Vector3f> vertices;
    std::vector<std::vector<int>> indices;

    if(!this->csgoutput.size())
        return;

    for(h=0; h<Bsplib::n_hulls; h++)
    {
        vertices.clear();
        indices.clear();

        filename = Utilslib::StripExtension(this->csgoutput.c_str());
        filename = filename + "_" + std::to_string(h);
        filename = Utilslib::AddExtension(filename.c_str(), "obj");
        ptr = fopen(filename.c_str(), "w");
        if(!ptr)
        {
            fprintf(stderr, "can't open file for writing \"%s\".\n", filename.c_str());
            continue;
        }

        for(e=0; e<this->ents.size(); e++)
        {
            curgeo = &this->ents[e].bsp[h].faces;
            for(f=0; f<curgeo->size(); f++)
            {
                indices.push_back({});
                for(v=0; v<(*curgeo)[f].poly.size(); v++)
                {
                    indices.back().push_back(vertices.size());
                    vertices.push_back((*curgeo)[f].poly[v]);
                }
            }
        }

        for(v=0; v<vertices.size(); v++)
            fprintf(ptr, "v %f %f %f\n", vertices[v][0], vertices[v][1], vertices[v][2]);
        for(f=0; f<indices.size(); f++)
        {
            fprintf(ptr, "f ");
            for(v=0; v<indices[f].size(); v++)
                fprintf(ptr, "%d ", indices[f][v] + 1);
            fprintf(ptr, "\n");
        }

        fclose(ptr);
    }
}

int BspBuilder::FindPlaneNum(Eigen::Vector3f n, float d)
{
    const float epsilon = 0.1;

    int i;

    bool flip;

    for(i=0; i<this->planes.size(); i++)
    {
        if((this->planes[i].n - n).squaredNorm() < epsilon * epsilon)
            flip = false;
        else if((this->planes[i].n + n).squaredNorm() < epsilon * epsilon)
            flip = true;
        else
            continue;

        if(!flip && fabsf(this->planes[i].d - d) > epsilon)
            continue;
        if(flip && fabsf(this->planes[i].d + d) > epsilon)
            continue;

        if(!flip)
            return i;
        return ~i;
    }

    this->planes.push_back(BspPlane());
    this->planes.back().n = n;
    this->planes.back().d = d;
    return this->planes.size() - 1;
}

int BspBuilder::FindTexinfoNum(const char* name, Eigen::Vector3f s, Eigen::Vector3f t, float sshift, float tshift)
{
    const float epsilon = 0.1;

    int i;

    assert(strlen(name) < Tpklib::max_tex_name && "name too long!");

    for(i=0; i<this->texinfos.size(); i++)
    {
        if(fabsf(this->texinfos[i].shift[0] - sshift) > epsilon)
            continue;
        if(fabsf(this->texinfos[i].shift[1] - tshift) > epsilon)
            continue;
        if((this->texinfos[i].basis[0] - s).squaredNorm() > epsilon * epsilon)
            continue;
        if((this->texinfos[i].basis[1] - t).squaredNorm() > epsilon * epsilon)
            continue;
        if(strcmp(this->texinfos[i].name, name))
            continue;
    }

    this->texinfos.push_back(TexInfo());
    strcpy(this->texinfos.back().name, name);
    this->texinfos.back().basis[0] = s;
    this->texinfos.back().basis[1] = t;
    this->texinfos.back().shift[0] = sshift;
    this->texinfos.back().shift[1] = tshift;
    return this->texinfos.size() - 1;
}

void BspBuilder::LoadMapFile(const char* path)
{
    int i, e, b, f;
    Maplib::entity_t *fent;
    Maplib::brush_t *fbr;
    Maplib::plane_t *fface;
    Entity *ent;
    Brush *br;
    BrFace *face;

    Maplib::MapFile mapfile;
    uint64_t startt, endt;
    int nent, nbrush, nface;

    startt = TIMEMS;

    mapfile = Maplib::MapFile::Load(path);
    if(!mapfile.ents.size())
        exit(1);

    nent = nbrush = nface = 0;

    this->ents.resize(mapfile.ents.size(), Entity(*this));
    for(e=0; e<this->ents.size(); e++, nent++)
    {
        fent = &mapfile.ents[e];
        ent = &this->ents[e];

        ent->pairs = fent->keys;
        ent->brushes[0].resize(fent->brushes.size());
        for(b=0; b<ent->brushes[0].size(); b++, nbrush++)
        {
            fbr = &fent->brushes[b];
            br = &ent->brushes[0][b];

            br->planes.resize(fbr->planes.size());
            for(f=0; f<br->planes.size(); f++, nface++)
            {
                fface = &fbr->planes[f];
                face = &br->planes[f];

                assert(fface->texname.size() < Tpklib::max_tex_name && "mapfile texname too long!");

                face->n = (fface->triplane[1] - fface->triplane[0]).cross(fface->triplane[2] - fface->triplane[0]).cast<float>();
                face->n.normalize();
                face->d = face->n.dot(fface->triplane[0].cast<float>());
                strcpy(face->texname, fface->texname.c_str());
                for(i=0; i<2; i++)
                {
                    face->texbasis[i] = fface->texbasis[i];
                    face->texshift[i] = fface->texoffs[i];
                }
            }
        }
    }

    endt = TIMEMS;

    if(this->verbose)
    {
        printf("loading of map \"%s\" complete in %llums:\n", path, endt - startt);
        printf("|- %d entities\n", nent);
        printf("|- %d brushes\n", nbrush);
        printf("`- %d brush faces\n", nface);
    }
}

void BspBuilder::CSG(void)
{
    this->ExpandHulls();
    this->CullInterior();
    this->WriteCSGFaces();
}