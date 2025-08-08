#include "HullDef.h"

#include <bsplib.h>
#include <parselib/Tokenizer.h>

HullDef::HullDef(void)
{
    this->LoadDefaults();
}

void HullDef::LoadDefaults(void)
{
    this->hulls[0][0] = Eigen::Vector3f(  0,   0,   0);
    this->hulls[0][1] = Eigen::Vector3f(  0,   0,   0);

    this->hulls[1][0] = Eigen::Vector3f(-16, -16, -24);
    this->hulls[1][1] = Eigen::Vector3f( 16,  16,  24);

    this->hulls[2][0] = Eigen::Vector3f(-32, -32, -24);
    this->hulls[2][1] = Eigen::Vector3f( 32,  32,  64);

    // TODO: Find a use for the 4th hull
    this->hulls[3][0] = Eigen::Vector3f(  0,   0,   0);
    this->hulls[3][1] = Eigen::Vector3f(  0,   0,   0);
}

bool HullDef::LoadFile(const char* path)
{
    const char *funcname = "HullDef::LoadFile";

    int i, j, k;

    Parselib::Tokenizer tknizer;
    Parselib::Tokenizer::token_t *tkn;

    if(!tknizer.EatFile(path))
        return false;

    tkn = &tknizer.tokens[0];
    if(!tknizer.ExpectValue(*tkn, "{", funcname))
        return false;
    tkn++;

    for(i=0; i<Bsplib::n_hulls; i++)
        for(j=0; j<2; j++)
            this->hulls[i][j] = Eigen::Vector3f::Zero();

    for(i=0; i<Bsplib::n_hulls; i++)
    {
        for(j=0; j<2; j++)
        {
            if(!tknizer.ExpectValue(*tkn, "(", funcname))
                return false;
            tkn++;

            for(k=0; k<3; k++)
            {
                if(!tknizer.ExpectType(*tkn, Parselib::Tokenizer::TOKEN_NUMBER, funcname))
                    return false;
                this->hulls[i][j][k] = std::stoi(tkn->val);
                tkn++;
            }

            if(!tknizer.ExpectValue(*tkn, ")", funcname))
                return false;
            tkn++;

            if(!j)
            {
                if(!tknizer.ExpectValue(*tkn, ",", funcname))
                    return false;
            }
            else
            {
                if(!tknizer.ExpectValue(*tkn, ";", funcname))
                    return false;
            }
            tkn++;
        }

        if(tkn->val == "}")
            break;
    }

    if(!tknizer.ExpectValue(*tkn, "}", funcname))
        return false;
    tkn++;

    if(!tknizer.ExpectType(*tkn, Parselib::Tokenizer::TOKEN_EOF, funcname))
        return false;

    return true;
}