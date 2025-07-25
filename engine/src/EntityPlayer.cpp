#include "EntityPlayer.h"

#include <math.h>

#include "EntityCamera.h"

void EntityPlayer::SetTargetAngle(void)
{
    Eigen::Vector3f forward, right;
    Eigen::Vector3f base, tip, n;
    Eigen::Vector3f p, dir;
    Eigen::Vector3f diff;
    float d, den, t;
    float x, y;

    base = Eigen::Vector3f(0, 0, 0);
    tip = Eigen::Vector3f(0, 0, 1);
    base = (ctlindices[0]->bone->noctl * TOHOMOGENOUS(base)).head<3>();
    tip = (ctlindices[0]->bone->noctl * TOHOMOGENOUS(tip)).head<3>();
    n = tip - base;
    d = n.dot(base);

    p = Game::GetGame().world.camera->pos;
    dir = Game::GetGame().world.camera->mousedir;

    // isn't this backwards? something to look into.
    den = n.dot(dir);
    if(fabsf(den) < 0.001)
        return;

    diff = base - p;
    t = diff.dot(n) / den;
    p += dir * t;

    right = Eigen::Vector3f(1, 0, 0);
    forward = Eigen::Vector3f(0, 1, 0);
    right = (ctlindices[0]->bone->noctl * TOHOMOGENOUS(right)).head<3>();
    forward = (ctlindices[0]->bone->noctl * TOHOMOGENOUS(forward)).head<3>();
    right = right - base;
    forward = forward - base;

    x = right.dot(p);
    y = forward.dot(p);

    yawtarget = RAD2DEG(atan2f(y, x)) + 180;
}

void EntityPlayer::Render(void)
{
    EntityStudio::Render();
}

void EntityPlayer::Tick(void)
{
    float accel;

    EntityStudio::Tick();

    SetTargetAngle();

    if(yawtarget - yaw > 180)
        yawtarget -= 360;

    if(yaw - yawtarget > 180)
        yawtarget += 360;

    yawvel -= 1.0 - friction;

    accel = (yawtarget - yaw) - yawvel;
    if(accel < -maxaccel)
        accel = -maxaccel;
    if(accel > maxaccel)
        accel = maxaccel;

    yawvel += accel;

    if(yawvel < -maxvel)
        yawvel = -maxvel;
    if(yawvel > maxvel)
        yawvel = maxvel;

    yaw += yawvel;

    while(yaw > 360)
        yaw -= 360;
    while(yaw < 0)
        yaw += 360;

    ctlindices[0]->cur = yaw + 180;
}

std::string EntityPlayer::GetModelName(void)
{
    return "models/tank";
}
