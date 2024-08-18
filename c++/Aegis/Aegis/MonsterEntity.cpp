#include "MonsterEntity.h"

#include "Game.h"

#include "mathutils.h"
#include "Quaternion.h"

MonsterEntity::MonsterEntity(BSPMap& map) : BaseEntity(map)
{
    
}

void MonsterEntity::Init()
{
	model.Load(GetStudioPath().c_str());
	model.map = map;
}

void MonsterEntity::Render()
{
    float vlen;

	model.camerapos = camerapos;

	glPushMatrix();

	glTranslatef(position.x, position.y, position.z);
	glRotatef(rotation.x * RAD2DEG, 1.0, 0.0, 0.0);
	glRotatef(rotation.y * RAD2DEG, 0.0, 1.0, 0.0);
	glRotatef(rotation.z * RAD2DEG, 0.0, 0.0, 1.0);

	model.render();

	glPopMatrix();

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);

    glVertex3f(position.x, position.y, position.z - hullbounds[GetClippingHull() << 1].z);
    glVertex3f(target.x, target.y, target.z - hullbounds[GetClippingHull() << 1].z);

    vlen = Vector3Length(vel);
    if (vlen > 0)
    {
        glColor3f(1, 1, 0);
        glVertex3f(position.x, position.y, position.z - hullbounds[GetClippingHull() << 1].z);
        glVertex3f(position.x + vel.x / vlen * 128, position.y + vel.y / vlen * 128, position.z - hullbounds[GetClippingHull() << 1].z);
    }

    glEnd();

    glEnable(GL_DEPTH);
}

void MonsterEntity::Think(float deltatime)
{
    int pathmode;
    
	model.Tick();
    
    pathmode = GetPathMode();
    if(pathmode < 0 || pathmode > PATH_GRAPH)
        return;
    
    if(!map->player)
        return;
    
    target = map->player->position;
    
    Gravity();
    FindFloor();
    
    if (floorplane.nType)
    {
        vel.x = vel.x - (vel.x * (float)(16.0 * ENGINE_TICKDUR));
        vel.y = vel.y - (vel.y * (float)(16.0 * ENGINE_TICKDUR));
    }
    Turn();
    
    Advance();
    
    rotation.z = atan2(vel.y, vel.x);
}

void MonsterEntity::Gravity()
{
    vec3_t p, v, planep;
    int safety = 8;
    
    v = {0, 0, -9.8 * ENGINE_TICKDUR};
    
    vel = vel + v;
    while(Sweep(position, vel, &p) && safety--)
    {
        planep = hitplane.vNormal * hitplane.fDist;
        p = position;
        p.z -= hullbounds[GetClippingHull() << 1].z;
        vel = planep + Slide(p - planep, vel, hitplane.vNormal) - position;
        vel.z += hullbounds[GetClippingHull() << 1].z;
    }
}

void MonsterEntity::FindFloor()
{
    vec3_t p;
    
    if(Sweep(position + vel, {0, 0, -1}, &p))
        floorplane = hitplane;
    else
        floorplane.nType = 0;
}

void MonsterEntity::Turn()
{
    const float maxaccel = 10 * GetMaxSpeed() * ENGINE_TICKDUR;
    
    vec2_t wishdir;
    vec2_t vel2;
    vec3_t add;
    float movespeed, addspeed;
    
    wishdir = NormalizeVector2({target.x - position.x, target.y - position.y});
    vel2 = {vel.x, vel.y};
    movespeed = wishdir.x * vel.x + wishdir.y * vel.y;
    addspeed = GetMaxSpeed() - movespeed;
    if(addspeed < 0)
        addspeed = 0;
    if(addspeed > maxaccel)
        addspeed = maxaccel;
    
    add = {wishdir.x * addspeed, wishdir.y * addspeed, 0};
    vel = vel + add;
}

void MonsterEntity::Advance()
{
    vec3_t p, v, planep;
    int safety = 8;

    if(!CanAdvance(position, vel))
        CantAdvance();

    while (Sweep(position, vel, &p) && safety--)
    {
        planep = hitplane.vNormal * hitplane.fDist;
        p = position;
        p.z -= hullbounds[GetClippingHull() << 1].z;
        vel = planep + Slide(p - planep, vel, hitplane.vNormal) - p;
    }

    position = position + vel;
}

void MonsterEntity::CantAdvance()
{
    float angle, leftcos, rightcos, r;
    vec3_t leftvel, rightvel;
    
    angle = 45.0 * DEG2RAD; // 1 degree left
    leftvel.x = vel.x * cosf(angle) - vel.y * sinf(angle);
    leftvel.y = vel.x * sinf(angle) + vel.y * cosf(angle);
    leftvel.z = 0;
    if(CanAdvance(position, leftvel))
    {
        vel = leftvel;
        return;
    }
    leftcos = hitplane.vNormal.x * leftvel.x + hitplane.vNormal.y * leftvel.y;
    
    angle = -45.0 * DEG2RAD; // 1 degree right
    rightvel.x = vel.x * cosf(angle) - vel.y * sinf(angle);
    rightvel.y = vel.x * sinf(angle) + vel.y * cosf(angle);
    rightvel.z = 0;
    if(CanAdvance(position, rightvel))
    {
        vel = rightvel;
        return;
    }
    rightcos = hitplane.vNormal.x * rightvel.x + hitplane.vNormal.y * rightvel.y;
    
    if(rightcos < leftcos)
    {
        vel.x = rightvel.x;
        vel.y = rightvel.y;
        return;
    }
    
    vel.x = leftvel.x;
    vel.y = leftvel.y;
}

bool MonsterEntity::CanAdvance(vec3_t p, vec3_t v)
{
    const float maxslope = 1.4; // Around 55 degrees
    const float mincos = 0.57;  // Around 55 degrees
    
    float cos;
    vec3_t end;
    vec2_t vel2;
    
    if(Sweep(position, vel, &end))
    {
        cos = DotProduct(hitplane.vNormal, {0, 0, 1});
        
        if(cos < mincos)
            return false; // Too steep upwards
        
        return true;
    }
    else
        end = position + vel;
    
    vel2 = {vel.x, vel.y};
    if(!Sweep(position, {0, 0, -maxslope * Vector2Length(vel2)}, &end))
        return false; // Too steep downwards
    
    return true;
}

bool MonsterEntity::Sweep(vec3_t p, vec3_t v, vec3_t* result)
{
    vec3_t n;
    
    p.z -= hullbounds[GetClippingHull() << 1].z;
    
    if(map->FineRaycast(p, p + v, result, &n, GetClippingHull()))
    {
        hitplane.vNormal = n;
        hitplane.fDist = DotProduct(*result, n);
        hitplane.nType = 1;
        return true;
    }
    
    return false;
}

vec3_t MonsterEntity::Slide(vec3_t p, vec3_t v, vec3_t n)
{
    float pdist, edist, t, vlen;
    vec3_t end;
    
    end = p + v;
    pdist = DotProduct(p, n);
    edist = DotProduct(end, n);
    
    if(pdist * edist > 0 || edist == 0)
        return end;
    
    t = -DotProduct(p, n) / DotProduct(v, n);
    p = Vector3Lerp(p, end, t);
    v = end - p;
    vlen = Vector3Length(v);
    v = v - n * DotProduct(v, n);
    if (v.x != 0 || v.y != 0 || v.z != 0)
    {
        v = NormalizeVector3(v);
        v = v * vlen;
    }
    return p + v;
}

std::string MonsterEntity::GetStudioPath()
{
    return Game::GetGame().gamedir + "/models/missingno/missingno.mdl";
}

int MonsterEntity::GetPathMode()
{
    return PATH_NONE;
}

int MonsterEntity::GetClippingHull()
{
    return 0;
}

float MonsterEntity::GetMaxSpeed()
{
    return 0;
}
