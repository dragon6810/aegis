#include "EntityPlayer.h"

#include "EntityCamera.h"

void EntityPlayer::SetTargetAngle(void)
{
    Vector3 forward, right;
    Vector3 base, tip, n;
    Vector3 p, dir;
    Vector3 diff;
    float d, den, t;
    float x, y;

    base = Vector3(0, 0, 0);
    tip = Vector3(0, 0, 1);
    base = ctlindices[0]->bone->noctl * base;
    tip = ctlindices[0]->bone->noctl * tip;
    n = tip - base;
    d = Vector3::Dot(n, base);

    p = Game::GetGame().world.camera->pos;
    dir = Game::GetGame().world.camera->mousedir;

    den = Vector3::Dot(n, dir);
    if(fabsf(den) < 0.001)
        return;

    diff = base - p;
    t = Vector3::Dot(diff, n) / den;
    p = Vector3::Lerp(p, p + dir, t);

    right = Vector3(1, 0, 0);
    forward = Vector3(0, 1, 0);
    right = ctlindices[0]->bone->noctl * right;
    forward = ctlindices[0]->bone->noctl * forward;
    right = right - base;
    forward = forward - base;

    x = Vector3::Dot(right, p);
    y = Vector3::Dot(forward, p);

    ctlindices[0]->cur = atan2f(y, x) * RAD2DEG;
}

void EntityPlayer::XRender(void)
{
    // Nothing
}

void EntityPlayer::XTick(void)
{
    

    SetTargetAngle();
}

std::string EntityPlayer::GetModelName(void)
{
    return "models/tank";
}
