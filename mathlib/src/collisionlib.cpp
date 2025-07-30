#include <mathlib.h>

Mathlib::hitresult_t Mathlib::RayCuboid(Eigen::Vector3f o, Eigen::Vector3f r, Eigen::Vector3f cuboid, float cuboidradius)
{
    const float epsilon = 0.01;

    int i;

    Eigen::Vector3f bb[2], n;
    hitresult_t result;
    float t[2], tbounds[2];

    result = {};

    bb[0] = cuboid - cuboidradius * Eigen::Vector3f::Ones();
    bb[1] = cuboid + cuboidradius * Eigen::Vector3f::Ones();
    tbounds[0] = -INFINITY;
    tbounds[1] = INFINITY;

    for(i=0; i<3; i++)
    {
        if(fabsf(r[i]) < epsilon)
        {
            if(o[i] < bb[0][i] || o[i] > bb[1][i])
            {
                result.hit = false;
                return result;
            }
            continue;
        }

        t[0] = (bb[0][i] - o[i]) / r[i];
        t[1] = (bb[1][i] - o[i]) / r[i];
        n = Eigen::Vector3f::Zero();

        n[i] = -1;
        if(t[0] > t[1])
        {
            std::swap(t[0], t[1]);
            n[i] = 1;
        }

        if(!i || t[0] > tbounds[0])
        {
            tbounds[0] = t[0];
            result.normal = n;
        }

        if(!i || t[1] < tbounds[1])
            tbounds[1] = t[1];
    }

    if(tbounds[1] < tbounds[0] || tbounds[1] < 0)
    {
        result.hit = false;
        return result;
    }

    result.hit = true;
    result.pos = o + r * tbounds[0];
    return result;
}