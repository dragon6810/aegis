#include <bsp/bsp.h>

#include <math.h>
#include <string.h>

#include <std/assert/assert.h>
#include <cli/cli.h>

bool bsp_findtexinfo_texinfosame(const bsp_texinfo_t* a, const bsp_texinfo_t* b)
{
    const float epsilon = 0.01;

    int i, j;

    assert(a);
    assert(b);

    if(strcmp(a->texname, b->texname))
        return false;

    for(i=0; i<2; i++)
    {
        if(fabsf(a->shift[i] - b->shift[i]) > epsilon)
            return false;
        for(j=0; j<3; j++)
        {
            if(fabsf(a->basis[i][j] - b->basis[i][j]) > epsilon)
                return false;
        }
    }

    return true;
}

int bsp_findtexinfo(const bsp_texinfo_t* texinfo)
{
    int i;

    assert(texinfo);

    for(i=0; i<bsp_ntexinfos; i++)
    {
        if(bsp_findtexinfo_texinfosame(texinfo, &bsp_texinfos[i]))
            return i;
    }

    if(bsp_ntexinfos >= MAX_MAP_TEXINFO)
        cli_error(true, "map exceeds max texinfos, max is %d\n", MAX_MAP_TEXINFO);

    memcpy(&bsp_texinfos[bsp_ntexinfos], texinfo, sizeof(bsp_texinfo_t));
    return bsp_ntexinfos++;
}