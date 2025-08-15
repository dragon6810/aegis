#include "Write.h"

#include <stdio.h>

#include <bsplib.h>
#include <utilslib.h>

#include "Bsp.h"

Bsplib::BspFile file;

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

void Write(const char* path)
{
    uint64_t startt, endt;

    printf("---- Write ----\n");

    startt = TIMEMS;

    file = Bsplib::BspFile();

    MakeEntString();

    file.Write(path);

    endt = TIMEMS;
    printf("Write done in %llums.\n", endt - startt);
}