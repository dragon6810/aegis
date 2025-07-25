#pragma once

#include "mathlib.h"

#include <array>
#include <vector>

#include <Eigen/Dense>

namespace Mathlib
{
    typedef enum
    {
        SIDE_BACK=0,
        SIDE_FRONT,
        SIDE_ON,
        SIDE_CROSS,
        SIDE_COUNT,
    } planeside_e;

    template <int R>
    using Poly = std::vector<Eigen::Vector<float, R>>;
    template <int R>
    using Tri = std::array<Eigen::Vector<float, R>, 3>;

    /*
        will return a poly clipped to a given side of the given separator
        seperator is e.g. a plane for R=3, line for R=2
        side must be SIDE_BACK or SIDE_FRONT
        if the poly is clipped out of existence, the returned poly will have n=0 points
    */
    template <int R>
    Poly<R> ClipPoly(Poly<R> poly, Eigen::Vector<float, R> n, float d, planeside_e side);

    /*
        checks if the distance between every corresponding point is below epsilon.
    */
    template <int R>
    bool EquivalentPolys(Poly<R> a, Poly<R> b, float epsilon=0.01);

    /*
        creates a quad with radius maxrange along the given plane.
        wound counter-clockwise.
    */
    Poly<3> FromPlane(Eigen::Vector3f n, float d, float maxrange=8192);
};

#include "polylib.tpp"