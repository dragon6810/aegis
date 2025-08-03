#include <fgdlib.h>

Fgdlib::FgdFile::FgdFile()
{
    this->ents.clear();
}

int Fgdlib::FgdFile::FindBaseClass(const char* name) const
{
    int i;

    for(i=0; i<this->ents.size(); i++)
    {
        if(ents[i].type != EntityDef::ENTTYPE_BASE)
            continue;
        
        if(strcmp(name, this->ents[i].classname.c_str()))
            continue;

        break;
    }

    if(i >= this->ents.size())
        return -1;

    return i;
}

Fgdlib::FgdFile Fgdlib::FgdFile::Load(std::string path)
{
    Parselib::Tokenizer::token_t *tkn;

    FgdFile file;
    Parselib::Tokenizer tknizer;
    std::optional<EntityDef> ent;

    file = FgdFile();

    tknizer.EatFile(path.c_str());
    
    tkn = tknizer.tokens.data();

    while(tkn->type != Parselib::Tokenizer::TOKEN_EOF)
    {
        ent = EntityDef::Load(&tkn, file);
        if(!ent.has_value())
            return file;
        file.entclasses[ent.value().type].insert(file.ents.size());
        file.ents.push_back(ent.value());
    }

    return file;
}