#pragma once

#include <Eigen/Dense>
#include <GL/glew.h>

class Viewport
{
public:
    typedef enum 
    {
        ORTHOX  = 0,
        ORTHOY  = 1,
        ORTHOZ  = 2,
        FREECAM = 3,
        NTYPES  = 4,
    } viewporttype_e;

    Eigen::Vector3f pos;
    Eigen::Vector3f rot; // freecam only; roll pitch yaw
    float zoom = 128.0f; // ortho only
    float fov = 60.0f;   // freccam only
    bool wireframe = true;
    viewporttype_e type = ORTHOX;

    GLuint fbo = 0, tex = 0, depth = 0;
    Eigen::Vector2i canvassize = Eigen::Vector2i(0, 0);

    // outbasis is forward, right, up
    void GetViewBasis(Eigen::Vector3f outbasis[3]) const;
    // clickpos in ndc
    void GetRay(Eigen::Vector2f clickpos, Eigen::Vector3f* outo, Eigen::Vector3f* outr) const;
};