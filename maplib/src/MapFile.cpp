#include <maplib.h>

#include <regex>

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
    int i, j;
    std::sregex_iterator e, k, b, p;

    MapFile map;
    FILE *ptr;
    std::regex entregex, keyregex, brregex, plregex;
    uint64_t size;
    std::string content, entcontent, brcontent;
    entity_t ent;
    brush_t br;
    plane_t pl;

    map = MapFile();

    ptr = fopen(path.c_str(), "r");
    if(!ptr)
    {
        fprintf(stderr, "Maplib::MapFile::Load: couldn't open file for reading \"%s\".\n", path.c_str());
        return map;
    }

    entregex = std::regex(R"(\{\s*([\s\S]*?)\s*\})");
    keyregex = std::regex(R"REGEX("([^"]+)"\s*:\s*"([^"]*)")REGEX"); 
    brregex = std::regex(R"(\{\s*((?:.|\n)*?)\s*\})"); 
    plregex = std::regex
    (
        R"(\(\s*(-?\d+)\s+(-?\d+)\s+(-?\d+)\s*\)\s*"
          R"\(\s*(-?\d+)\s+(-?\d+)\s+(-?\d+)\s*\)\s*"
          R"\(\s*(-?\d+)\s+(-?\d+)\s+(-?\d+)\s*\)\s*"
          R"\[\s*([-\d.eE]+)\s+([-\d.eE]+)\s+([-\d.eE]+)\s+([-\d.eE]+)\s*\]\s*"
          R"\[\s*([-\d.eE]+)\s+([-\d.eE]+)\s+([-\d.eE]+)\s+([-\d.eE]+)\s*\]\s*"
          R"\"([^\"]+)\")",
        std::regex::optimize
    );

    for(e=std::sregex_iterator(content.begin(), content.end(), entregex); e!=std::sregex_iterator(); e++)
    {
        entcontent = (*e)[1];

        ent = {};
        
        for(k=std::sregex_iterator(entcontent.begin(), entcontent.end(), keyregex); k!=std::sregex_iterator(); k++)
            ent.keys[(*k)[1]] = (*k)[2];
        
        for(b=std::sregex_iterator(entcontent.begin(), entcontent.end(), brregex); b!=std::sregex_iterator(); b++)
        {
            brcontent = (*b)[1];

            br = {};

            for(p=std::sregex_iterator(brcontent.begin(), brcontent.end(), plregex); p!=std::sregex_iterator(); p++)
            {
                pl = {};

                for(i=0; i<3; i++)
                    for(j=0; j<3; j++)
                        pl.triplane[i][j] = std::stoi((*p)[1 + i * 3 + j]);

                for(i=0; i<2; i++)
                {
                    for(j=0; j<3; j++)
                        pl.texbasis[i][j] = std::stof((*p)[10 + i * 4 + j]);
                    pl.texoffs[i] = std::stof((*p)[10 + i * 4 + j]);
                }
                
                pl.texname = (*p)[18];

                br.planes.push_back(pl);
            }

            ent.brushes.push_back(br);
        }
        map.ents.push_back(ent);
    }

    fclose(ptr);
    return map;
}