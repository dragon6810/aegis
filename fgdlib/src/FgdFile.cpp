#include <fgdlib.h>

Fgdlib::FgdFile::FgdFile()
{
    this->ents.clear();
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
        ent = EntityDef::Load(&tkn);
        if(!ent.has_value())
            return file;
        file.ents.push_back(ent.value());
    }

    return file;
}