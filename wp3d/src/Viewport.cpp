#include "Viewport.h"

#include <mathlib.h>

void Viewport::GetViewBasis(Eigen::Vector3f outbasis[3]) const
{
    Eigen::Quaternionf q;

    assert(outbasis);

    switch(this->type)
    {
        case Viewport::ORTHOX:
            outbasis[0] = Eigen::Vector3f(-1, 0, 0);
            outbasis[1] = Eigen::Vector3f(0, 1, 0);
            outbasis[2] = Eigen::Vector3f(0, 0, 1);
            break;
        case Viewport::ORTHOY:
            outbasis[0] = Eigen::Vector3f(0, -1, 0);
            outbasis[1] = Eigen::Vector3f(-1, 0, 0);
            outbasis[2] = Eigen::Vector3f(0, 0, 1);
            break;
        case Viewport::ORTHOZ:
            outbasis[0] = Eigen::Vector3f(0, 0, -1);
            outbasis[1] = Eigen::Vector3f(1, 0, 0);
            outbasis[2] = Eigen::Vector3f(0, 1, 0);
            break;
        case Viewport::FREECAM:
            q = Mathlib::FromEuler(this->rot);
            outbasis[0] = q * Eigen::Vector3f(1, 0, 0);
            outbasis[1] = q * Eigen::Vector3f(0, -1, 0);
            outbasis[2] = q * Eigen::Vector3f(0, 0, 1);
            break;
        default:
            break;
    }
}

void Viewport::GetRay(Eigen::Vector2f clickpos, Eigen::Vector3f* outo, Eigen::Vector3f* outr) const
{
    Eigen::Vector3f o, r, basis[3];
    Eigen::Vector2f size;

    this->GetViewBasis(basis);

    o = r = Eigen::Vector3f::Zero();
    switch(this->type)
    {
    case FREECAM:
        size[1] = tanf(DEG2RAD(this->fov) / 2.0);
        size[0] = size[1] * (float) this->canvassize[0] / (float) this->canvassize[1];
        r = basis[0] + size[0] * basis[1] * clickpos[0] + size[1] * basis[2] * clickpos[1];
        r.normalize();
        o = this->pos;

        break;
    case ORTHOX:
    case ORTHOY:
    case ORTHOZ:
        size[1] = this->zoom;
        size[0] = size[1] * (float) this->canvassize[0] / (float) this->canvassize[1];
        r = basis[0];
        o = this->pos + size[0] * basis[1] * clickpos[0] + size[1] * basis[2] * clickpos[1];
        printf("mouse (%f %f)\n", clickpos[0], clickpos[1]);
        printf("o: (%f %f %f), r: (%f %f %f)\n", o[0], o[1], o[2], r[0], r[1], r[2]);

        break;
    default:
        break;
    }

    if(outo)
        *outo = o;
    if(outr)
        *outr = r;
}