#include <maplib.h>

#include <parselib/Tokenizer.h>

Maplib::MapFile::MapFile()
{
    this->ents.clear();
}

bool Maplib::MapFile::Write(std::string path)
{
    int i, e, b, p;
    std::unordered_map<std::string, std::string>::iterator it;
    entity_t *ent;
    brush_t *br;
    plane_t *pl;

    FILE *ptr;

    ptr = fopen(path.c_str(), "w");
    if(!ptr)
    {
        fprintf(stderr, "Maplib::MapFile::Write: couldn't open file for writing \"%s\".\n", path.c_str());
        return false;
    }

    for(e=0, ent=this->ents.data(); e<this->ents.size(); e++, ent++)
    {
        fprintf(ptr, "{\n");

        for(it=ent->keys.begin(); it!=ent->keys.end(); it++)
            fprintf(ptr, "    \"%s\": \"%s\"\n", it->first.c_str(), it->second.c_str());

        for(b=0, br=ent->brushes.data(); b<ent->brushes.size(); b++, br++)
        {
            fprintf(ptr, "    {\n");

            for(p=0, pl=br->planes.data(); p<br->planes.size(); p++, pl++)
            {
                fprintf(ptr, "        ");
                for(i=0; i<3; i++)
                    fprintf(ptr, "( %d %d %d ) ", pl->triplane[i][0], pl->triplane[i][1], pl->triplane[i][2]);
                for(i=0; i<2; i++)
                    fprintf(ptr, "[ %f %f %f %f ] ", pl->texbasis[i][0], pl->texbasis[i][1], pl->texbasis[i][2], pl->texoffs[i]);
                fprintf(ptr, "\"%s\"\n", pl->texname.c_str());
            }

            fprintf(ptr, "    }\n");
        }

        fprintf(ptr, "}\n");
    }

    fclose(ptr);
    return true;
}

Maplib::MapFile Maplib::MapFile::Load(std::string path)
{
    int i;

    MapFile map;
    Parselib::Tokenizer tknizer;
    entity_t ent;
    brush_t br;
    plane_t pl;

    map = MapFile();

    tknizer.EatFile(path.c_str());
    for(i=0; i<tknizer.tokens.size(); i++)
    {
        printf("token: \"%s\".\n", tknizer.tokens[i].val.c_str());
    }

    return map;
}
