#include "TpkScript.h"

#include <tpklib.h>

bool TpkScript::Write(const char* path, int compress)
{
    int i;
    std::unordered_map<std::string, std::string>::iterator it;

    Tpklib::TpkFile file;
    std::optional<Tpklib::TpkTex> tex;
    bool success;

    for(i=0; i<this->textures.size(); i++)
    {
        tex = LoadBmp(this->textures[i].first.c_str(), this->textures[i].second.c_str());
        if(!tex.has_value())
            return false;
        file.tex[this->textures[i].second] = tex.value();
        printf("read BMP file \"%s\".\n", this->textures[i].first.c_str());
    }
    
    success = file.Write(path, compress);
    if(success)
        printf("wrote TPK file \"%s\".\n", path);
    return success;
}

std::optional<Tpklib::TpkTex> TpkScript::LoadBmp(const char* filename, const char* texname)
{
    int i;

    FILE *ptr;
    Tpklib::TpkTex tex;
    uint16_t magic;
    uint32_t datastart;
    uint32_t dibsize;
    uint32_t dimensions[2];
    uint16_t bitdepth;
    uint32_t compression;
    uint32_t ncolors;
    uint32_t col;

    ptr = fopen(filename, "rb");
    if(!ptr)
    {
        fprintf(stderr, "TpkScript::LoadBmp: can't open file for reading \"%s\".\n", filename);
        return std::optional<Tpklib::TpkTex>();
    }

    fread(&magic, sizeof(magic), 1, ptr);
    if(magic != 0x4D42)
    {
        fprintf(stderr, "TpkScript::LoadBmp: invalid or corrupted BMP file \"%s\".\n", filename);
        fclose(ptr);
        return std::optional<Tpklib::TpkTex>();
    }

    fseek(ptr, 8, SEEK_CUR);
    fread(&datastart, sizeof(datastart), 1, ptr);

    fread(&dibsize, sizeof(dibsize), 1, ptr);
    if(dibsize != 40)
    {
        fprintf(stderr, "TpkScript::LoadBmp: unsupported DIB type for BMP file \"%s\".\n", filename);
        fclose(ptr);
        return std::optional<Tpklib::TpkTex>();
    }

    fread(dimensions, sizeof(dimensions[0]), 2, ptr);
    fseek(ptr, 2, SEEK_CUR);
    fread(&bitdepth, sizeof(bitdepth), 1, ptr);
    if(bitdepth != 8)
    {
        fprintf(stderr, "TpkScript::LoadBmp: unsupported bit depth for BMP file \"%s\".\n", filename);
        fclose(ptr);
        return std::optional<Tpklib::TpkTex>();
    }
    fread(&compression, sizeof(compression), 1, ptr);
    if(compression)
    {
        fprintf(stderr, "TpkScript::LoadBmp: unsupported compression mode for BMP file \"%s\".\n", filename);
        fclose(ptr);
        return std::optional<Tpklib::TpkTex>();
    }
    fseek(ptr, 12, SEEK_CUR);
    fread(&ncolors, sizeof(ncolors), 1, ptr);
    fseek(ptr, 4, SEEK_CUR);

    for(i=0; i<ncolors; i++)
    {
        fread(&col, sizeof(col), 1, ptr);
        tex.palette[i][0] = (col & 0x0000FF00) >>  8;
        tex.palette[i][1] = (col & 0x00FF0000) >> 16;
        tex.palette[i][2] = (col & 0xFF000000) >> 24;
    }

    tex.size[0] = dimensions[0];
    tex.size[1] = dimensions[1];
    strcpy(tex.name, texname);
    tex.palettedata.resize(dimensions[0] * dimensions[1]);
    fseek(ptr, datastart, SEEK_SET);
    fread(tex.palettedata.data(), dimensions[0] * dimensions[1], 1, ptr);

    fclose(ptr);

    return std::optional<Tpklib::TpkTex>(tex);
}

bool TpkScript::LoadScript(const char* path)
{
    return this->script.ParseFile(path);
}

bool TpkScript::RunScript(void)
{
    int i;
    Scriptlib::ScriptStatement *statement;

    std::string curfile;
    std::string compress;

    curfile = "";
    compress = "1";
    for(i=0; i<this->script.statements.size(); i++)
    {
        statement = &this->script.statements[i];

        switch(statement->type)
        {
        case Scriptlib::ScriptStatement::STATEMENT_ASSIGN:
            if(statement->lident == "curfile")
                curfile = statement->args[0];
            else if (statement->lident == "compress")
                compress = statement->args[0];
            else
            {
                fprintf(stderr, "TpkScript::RunScript: undeclared indentifier $%s.\n", statement->lident.c_str());
                return false;
            }
            break;
        case Scriptlib::ScriptStatement::STATEMENT_CALL:
            if(statement->lident == "addtex")
            {
                if(statement->args.size() != 2)
                {
                    fprintf(stderr, "TpkScript::RunScript: expected exactly 2 arguments to function @addtex.\n");
                    return false;
                }
                this->textures.push_back(std::pair<std::string, std::string>(statement->args[0], statement->args[1]));
            }
            else if(statement->lident == "writefile")
            {
                if(statement->args.size())
                {
                    fprintf(stderr, "TpkScript::RunScript: expected exactly 0 arguments to function @writefile.\n");
                    return false;
                }
                if(!this->Write(curfile.c_str(), std::stoi(compress)))
                    return false;
                this->textures.clear();
            }
            break;
        default:
            break;
        }
    }

    return true;
}