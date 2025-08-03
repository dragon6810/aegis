#include "TpkScript.h"

bool TpkScript::Write(const char* path)
{
    printf("TODO: Write\n");
    return true;
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
                if(!this->Write(curfile.c_str()))
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