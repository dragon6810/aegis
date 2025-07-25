#include "polylib.h"

Mathlib::Poly<3> Mathlib::FromPlane(Eigen::Vector3f n, float d, float maxrange)
{
    const Eigen::Vector2f winding[4] = 
    {
        { -1, -1},
        { 1, -1},
        { 1, 1},
        { -1, 1},
    };

    int i, j;

    int axis;
    float bestaxis;
    Mathlib::Poly<3> poly;
    Eigen::Vector3f basis[2]; // right, up
    Eigen::Vector3f p;
    Eigen::Vector3f dir;
    float t;

    for(i=0; i<3; i++)
    {
        if(!i || fabsf(n[i]) > fabsf(bestaxis))
        {
            axis = i;
            bestaxis = n[i];
        }
    }

    for(i=0; i<2; i++)
    {
        basis[i] = Eigen::Vector3f::Zero();
        basis[i][(axis+i)%3] = 1;
    }

    dir = Eigen::Vector3f::Zero();
    dir[axis] = 1;
    poly.resize(4);
    for(i=0; i<4; i++)
    {
        p = Eigen::Vector3f::Zero();
        for(j=0; j<2; j++)
            p += winding[i][j] * basis[j];
        
        t = (n.dot(p) - d) / -n.dot(dir);
        p += dir * t;
        poly[i] = p;
    }

    if(bestaxis < 0)
        std::reverse(poly.begin(), poly.end());

    return poly;
}