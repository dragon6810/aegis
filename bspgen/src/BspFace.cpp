#include "BspFace.h"

#include <assert.h>

#include "BspBuilder.h"

BspFace::BspFace(void)
{

}

BspFace::BspFace(const BrFace& brface, BspBuilder* builder)
{
    assert(builder);

    this->poly = brface.poly;
    this->planenum = builder->FindPlaneNum(brface.n, brface.d);
    this->texinfonum = builder->FindTexinfoNum(brface.texname, brface.texbasis[0], brface.texbasis[1], brface.texshift[0], brface.texshift[1]);
}