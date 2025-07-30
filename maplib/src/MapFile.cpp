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
    int i, j;
    Parselib::Tokenizer::token_t *tkn;

    MapFile map;
    Parselib::Tokenizer tknizer;
    entity_t ent;
    brush_t br;
    plane_t pl;
    std::string key, val;
    std::string cur;

    map = MapFile();

    tknizer.EatFile(path.c_str());
    
    tkn = tknizer.tokens.data();
    while(tkn <= &tknizer.tokens.back())
    {
        ent = {};

        if(tkn->val != "{")
        {
            fprintf(stderr, "Maplib::MapFile::Load: expected entity definition.\n");
            return map;
        }
        tkn++;
        if(tkn > &tknizer.tokens.back())
        {
            fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
            return map;
        }

        while(tkn->type == Parselib::Tokenizer::TOKEN_STRING)
        {
            key = tkn->val;
            key.erase(0, 1);
            key.pop_back();
            tkn++;
            if(tkn > &tknizer.tokens.back())
            {
                fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                return map;
            }

            if(tkn->val != ":")
            {
                fprintf(stderr, "Maplib::MapFile::Load: expected ':' after entity key.\n");
                return map;
            }
            tkn++;
            if(tkn > &tknizer.tokens.back())
            {
                fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                return map;
            }

            if(tkn->type != Parselib::Tokenizer::TOKEN_STRING)
            {
                fprintf(stderr, "Maplib::MapFile::Load: expected ':' after entity key.\n");
                return map;
            }
            val = tkn->val;
            val.erase(0, 1);
            val.pop_back();
            tkn++;
            if(tkn > &tknizer.tokens.back())
            {
                fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                return map;
            }

            ent.keys[key] = val;
        }

        while(tkn->val == "{")
        {
            br = {};

            tkn++;
            if(tkn > &tknizer.tokens.back())
            {
                fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                return map;
            }

            while(tkn->val == "(")
            {
                pl = {};

                for(i=0; i<3; i++)
                {
                    if(tkn->val != "(")
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: expected vector definition.\n");
                        return map;
                    }
                    tkn++;
                    if(tkn > &tknizer.tokens.back())
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                        return map;
                    }

                    for(j=0; j<3; j++)
                    {
                        if(tkn->type != Parselib::Tokenizer::TOKEN_NUMBER)
                        {
                            fprintf(stderr, "Maplib::MapFile::Load: expected vector entry.\n");
                            return map;
                        }

                        pl.triplane[i][j] = std::stoi(tkn->val);
                        tkn++;
                        if(tkn > &tknizer.tokens.back())
                        {
                            fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                            return map;
                        }
                    }

                    if(tkn->val != ")")
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: expected vector closure.\n");
                        return map;
                    }
                    tkn++;
                    if(tkn > &tknizer.tokens.back())
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                        return map;
                    }
                }

                for(i=0; i<2; i++)
                {
                    if(tkn->val != "[")
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: expected vector definition.\n");
                        return map;
                    }
                    tkn++;
                    if(tkn > &tknizer.tokens.back())
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                        return map;
                    }

                    for(j=0; j<3; j++)
                    {
                        if(tkn->type != Parselib::Tokenizer::TOKEN_NUMBER)
                        {
                            fprintf(stderr, "Maplib::MapFile::Load: expected vector entry.\n");
                            return map;
                        }

                        pl.texbasis[i][j] = std::stof(tkn->val);
                        tkn++;
                        if(tkn > &tknizer.tokens.back())
                        {
                            fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                            return map;
                        }
                    }

                    if(tkn->type != Parselib::Tokenizer::TOKEN_NUMBER)
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: expected vector entry.\n");
                        return map;
                    }

                    pl.texoffs[i] = std::stof(tkn->val);
                    tkn++;
                    if(tkn > &tknizer.tokens.back())
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                        return map;
                    }

                    if(tkn->val != "]")
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: expected vector closure.\n");
                        return map;
                    }
                    tkn++;
                    if(tkn > &tknizer.tokens.back())
                    {
                        fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                        return map;
                    }
                }

                if(tkn->type != Parselib::Tokenizer::TOKEN_STRING)
                {
                    fprintf(stderr, "Maplib::MapFile::Load: expected string.\n");
                    return map;
                }

                cur = tkn->val;
                cur.erase(0, 1);
                cur.pop_back();
                pl.texname = cur;
                tkn++;
                if(tkn > &tknizer.tokens.back())
                {
                    fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                    return map;
                }

                br.planes.push_back(pl);
            }

            if(tkn->val != "}")
            {
                fprintf(stderr, "Maplib::MapFile::Load: expected plane definition or brush closure.\n");
                return map;
            }
            tkn++;
            if(tkn > &tknizer.tokens.back())
            {
                fprintf(stderr, "Maplib::MapFile::Load: unexpected EOF.\n");
                return map;
            }

            ent.brushes.push_back(br);
        }

        if(tkn->val != "}")
        {
            fprintf(stderr, "Maplib::MapFile::Load: expected key definition, brush definition, or entity definition closure.\n");
            return map;
        }
        tkn++;

        map.ents.push_back(ent);
    }

    return map;
}
