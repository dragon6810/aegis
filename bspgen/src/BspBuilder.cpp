#include "BspBuilder.h"

#include <maplib.h>
#include <utilslib.h>

void BspBuilder::ExpandHulls(void)
{
    int i;

    for(i=0; i<this->ents.size(); i++)
        this->ents[i].ExpandBrushes(this->hulldef);
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
}