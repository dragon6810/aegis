#pragma once

template <int R>
Mathlib::Poly<R> Mathlib::ClipPoly(Mathlib::Poly<R> poly, Eigen::Vector<float, R> n, float d, planeside_e side)
{
    const float epsilon = 0.01;

    int i, j;

    std::vector<planeside_e> sides;
    std::vector<float> dists;
    Poly<R> clipped;
    Eigen::Vector<float, R> e[2], dir;
    float t;

    assert(side == SIDE_BACK || side == SIDE_FRONT);

    if(side == SIDE_FRONT)
    {
        n *= -1;
        d = -d;
    }

    sides.resize(poly.size() + 1);
    dists.resize(poly.size() + 1);
    for(i=0; i<poly.size(); i++)
    {
        dists[i] = poly[i].dot(n) - d;
        if(dists[i] < -epsilon)
            sides[i] = SIDE_BACK;
        else if(dists[i] > epsilon)
            sides[i] = SIDE_FRONT;
        else
            sides[i] = SIDE_ON;
    }
    sides[i] = sides[0];
    dists[i] = dists[0];

    clipped.clear();
    for(i=0; i<poly.size(); i++)
    {
        if(sides[i] == SIDE_BACK || sides[i] == SIDE_ON)
            clipped.push_back(poly[i]);
        if(sides[i] == SIDE_ON || sides[i+1] == SIDE_ON)
            continue;

        if((sides[i] == SIDE_BACK) == (sides[i+1] == SIDE_BACK))
            continue;

        for(j=0; j<2; j++)
            e[j] = poly[(i+j)%poly.size()];
        dir = (e[1] - e[0]).normalized();

        t = dists[i] / -n.dot(dir);
        clipped.push_back(e[0] + dir * t);
    }

    if(clipped.size() < 3)
        return Poly<R>();

    return clipped;
}

template <int R>
bool Mathlib::EquivalentPolys(Mathlib::Poly<R> a, Mathlib::Poly<R> b, float epsilon)
{
    int i;

    int offs;

    if(a.size() != b.size())
        return false;

    if(!a.size())
        return true;

    for(i=0; i<a.size(); i++)
    {
        if((b[i] - a[0]).norm() > epsilon)
            continue;

        offs = i;
        break;
    }

    if(i >= a.size())
        return false;

    for(i=0; i<a.size(); i++)
        if((b[(i + offs) % b.size()] - a[i]).norm() > epsilon)
            return false;

    return true;
}