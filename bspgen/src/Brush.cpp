#include "Brush.h"

void Brush::Polygonize(void)
{
    int i, j;

    std::vector<BrFace> newplanes;

    newplanes = this->planes;
    for(i=0; i<newplanes.size(); i++)
    {
        newplanes[i].poly = Mathlib::FromPlane(newplanes[i].n, newplanes[i].d);
        for(j=0; j<newplanes.size(); j++)
        {
            if(i == j)
                continue;

            newplanes[i].poly = Mathlib::ClipPoly<3>(newplanes[i].poly, newplanes[j].n, newplanes[j].d, Mathlib::SIDE_BACK);
        }
    }

    this->planes.clear();
    this->planes.reserve(newplanes.size());
    for(i=0; i<newplanes.size(); i++)
    {
        if(!newplanes[i].poly.size())
            continue;
        this->planes.push_back(newplanes[i]);
    }

    this->FindBB();
}

void Brush::FindBB(void)
{
    int i, j, k;

    for(i=0; i<this->planes.size(); i++)
    {
        for(j=0; j<this->planes[i].poly.size(); j++)
        {
            for(k=0; k<3; k++)
            {
                if((!i && !j) || this->planes[i].poly[j][k] < bb[0][k])
                    bb[0][k] = this->planes[i].poly[j][k];
                if((!i && !j) || this->planes[i].poly[j][k] > bb[1][k])
                    bb[1][k] = this->planes[i].poly[j][k];
            }
        }
    }
}

void Brush::Expand(const Eigen::Vector3f hull[2])
{
    const float epsilon = 0.01;

    int i, j, k;

    Eigen::Vector3f corner;
    BrFace newface;
    int sizebeforebb;

    this->Polygonize(); // solely for bounding box

    for(i=0; i<this->planes.size(); i++)
    {
        corner = Eigen::Vector3f::Zero();
        for(j=0; j<3; j++)
        {
            if(this->planes[i].n[j] < epsilon)
                corner[j] = hull[0][j];
            if(this->planes[i].n[j] > epsilon)
                corner[j] = hull[1][j];
        }

        this->planes[i].d += this->planes[i].n.dot(corner);
    }

    sizebeforebb = this->planes.size();
    this->planes.reserve(sizebeforebb + 6); // this is probably unnecessary because reallocation is improbable for most brushes
    for(i=0; i<3; i++)
    {
        for(j=0; j<2; j++)
        {
            newface = BrFace();
            newface.n[i] = j * 2 - 1;
            newface.d = (this->bb[j][i] + hull[j][i]) * (j * 2 - 1);

            // it really fucks things up if you add a coplanar face, so dont add it if it exists already
            for(k=0; k<sizebeforebb; k++)
                if((this->planes[k].n - newface.n).squaredNorm() < epsilon * epsilon && 
                    fabsf(this->planes[k].d - newface.d) < epsilon)
                    break;

            if(k < sizebeforebb)
                continue;

            this->planes.push_back(newface);
        }
    }

    this->Polygonize();
}

bool Brush::Overlaps(const Brush& otherbrush)
{
    int i;

    for(i=0; i<3; i++)
    {
        if(this->bb[0][i] > otherbrush.bb[1][i])
            return false;
        if(this->bb[1][i] < otherbrush.bb[0][i])
            return false;
    }

    return true;
}

void Brush::PopulateExterior(void)
{
    int i;

    this->exterior.resize(this->planes.size());
    for(i=0; i<this->planes.size(); i++)
        this->exterior[i] = this->planes[i];
}

void Brush::SeperateInOut(const Brush& otherbrush, bool priority)
{
    int f, p;

    Mathlib::planeside_e side;
    const Eigen::Vector3f *n;
    const float *d;
    Mathlib::Poly<3> back, front;

    this->interior = this->exterior;
    this->exterior.clear();

    for(p=0; p<otherbrush.planes.size(); p++)
    {
        for(f=0; f<this->interior.size(); f++)
        {
            n = &otherbrush.planes[p].n;
            d = &otherbrush.planes[p].d;

            side = Mathlib::PolySide(this->interior[f].poly, *n, *d);
            
            // entirely inside
            if(side == Mathlib::SIDE_BACK)
                continue;

            if(side == Mathlib::SIDE_ON || side == Mathlib::SIDE_FRONT)
            {
                if(side == Mathlib::SIDE_ON && priority)
                    continue;

                // entirely outside
                this->exterior.push_back(this->interior[f]);
                this->interior.erase(this->interior.begin() + f);
                f--;
                continue;
            }

            // cross
            this->exterior.push_back(this->interior[f]);
            this->exterior.back().poly = Mathlib::ClipPoly(this->exterior.back().poly, *n, *d, Mathlib::SIDE_FRONT);
            this->interior[f].poly = Mathlib::ClipPoly(this->interior[f].poly, *n, *d, Mathlib::SIDE_BACK);
        }
    }
}