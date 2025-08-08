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
                hullcounts[h] += this->ents[i].geometry[h].size();
            
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
    std::vector<Mathlib::Poly<3>> *curgeo;
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
            curgeo = &this->ents[e].geometry[h];
            for(f=0; f<curgeo->size(); f++)
            {
                indices.push_back({});
                for(v=0; v<(*curgeo)[f].size(); v++)
                {
                    indices.back().push_back(vertices.size());
                    vertices.push_back((*curgeo)[f][v]);
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

    this->ents.resize(mapfile.ents.size());
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