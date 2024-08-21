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
    target = dest = map->player->position;
    attention = 0;
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
    
    if(!Game::GetGame().aidebug)
        return;

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);

    glVertex3f(position.x, position.y, position.z - hullbounds[GetClippingHull() << 1].z);
    glVertex3f(target.x, target.y, target.z - hullbounds[GetClippingHull() << 1].z);

    glColor3f(1, 1, 0);
    glVertex3f(position.x, position.y, position.z - hullbounds[GetClippingHull() << 1].z);
    glVertex3f(position.x + cosf(realyaw) * 128, position.y + sinf(realyaw) * 128, position.z - hullbounds[GetClippingHull() << 1].z);
    
    glColor3f(0, 1, 0);
    glVertex3f(position.x, position.y, position.z - hullbounds[GetClippingHull() << 1].z);
    glVertex3f(position.x + cosf(idealyaw) * 128, position.y + sinf(idealyaw) * 128, position.z - hullbounds[GetClippingHull() << 1].z);
    
    glEnd();

    glEnable(GL_DEPTH);
}

void MonsterEntity::Think(float deltatime)
{
    vec3_t p;
    int pathmode;
    
	model.Tick();
    
    pathmode = GetPathMode();
    if(pathmode < 0 || pathmode > PATH_GRAPH)
        return;
    
    if(!map->player)
        return;
    
    target.z = dest.z = position.z;
    
    Gravity();
    FindFloor();
    
    if (floorplane.nType)
    {
        vel.x = vel.x - (vel.x * (float)(16.0 * ENGINE_TICKDUR));
        vel.y = vel.y - (vel.y * (float)(16.0 * ENGINE_TICKDUR));
    }
    
    if(!attention--)
    {
        LockOn();
        attention = ATTENTION_SPAN;
    }
    
    Turn();
    Advance();
    
    rotation.z = realyaw;
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
    const float turnspeed = M_PI * 2 * ENGINE_TICKDUR; // One second for full rotation
    
    vec2_t wishdir;
    vec2_t vel2;
    vec3_t add;
    float movespeed, addspeed, diff;
    
    wishdir.x = cosf(idealyaw);
    wishdir.y = sinf(idealyaw);
    vel2 = {vel.x, vel.y};
    movespeed = wishdir.x * vel.x + wishdir.y * vel.y;
    addspeed = GetMaxSpeed() - movespeed;
    if(addspeed < 0)
        addspeed = 0;
    if(addspeed > maxaccel)
        addspeed = maxaccel;
    
    add = {wishdir.x * addspeed, wishdir.y * addspeed, 0};
    vel = vel + add;
    
    diff = (idealyaw - realyaw) * RAD2DEG;
    diff = ((int) diff + 180) % 360 - 180;
    diff *= DEG2RAD;
    
    if(diff > 0)
        realyaw += minf(diff, turnspeed);
    else if (diff < 0)
        realyaw += maxf(diff, -turnspeed);
}

void MonsterEntity::LockOn() // Here's the doom part
{
    int olddir;
    int wantdir;
    int diff1, diff2;
    vec3_t wantv;
    
    olddir = (int) round(idealyaw * RAD2DEG / 45) * 45;
    wantdir = (int) round(atan2f(target.y - position.y, target.x - position.x) * RAD2DEG / 45) * 45;
    
    olddir = (olddir + 45 + 360) % 360;
    diff1 = abs(wantdir - olddir);
    olddir = (olddir - 90 + 360) % 360;
    diff2 = abs(wantdir - olddir);
    olddir = (olddir + 45 + 360) % 360;
    
    diff1 = minf(diff1, 360 - diff1);
    diff2 = minf(diff2, 360 - diff2);
    
    if(diff1 < diff2)
        wantdir = (olddir + 45) % 360;
    
    if(diff2 < diff1)
        wantdir = (olddir - 45 + 360) % 360;
    
    wantv.x = cosf((float) wantdir * DEG2RAD);
    wantv.y = sinf((float) wantdir * DEG2RAD);
    wantv.z = 0;
    
    if(CanAdvance(position, wantv))
    {
        idealyaw = (float) wantdir * DEG2RAD;
        return;
    }
}

void MonsterEntity::Advance()
{
    vec3_t p, planep;
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

void MonsterEntity::CantAdvance() // More doom stuff
{
    int i;
    
    int rand;
    float newyaw;
    vec3_t newv;
    
    rand = Game::GetGame().P_Random(0, 2) > 1;
    
    if(rand)
        newyaw = idealyaw + 45 * DEG2RAD;
    else
        newyaw = idealyaw - 45 * DEG2RAD;
    
    newv.x = cosf(newyaw);
    newv.y = sinf(newyaw);
    newv.z = 0;
    
    for(i=0; i<6 && CanAdvance(position, newv); i++)
    {
        switch(i)
        {
            case 0:
                if(rand)
                    newyaw = idealyaw - 45 * DEG2RAD;
                else
                    newyaw = idealyaw + 45 * DEG2RAD;
                break;
            case 1:
                if(rand)
                    newyaw = idealyaw - 90 * DEG2RAD;
                else
                    newyaw = idealyaw + 90 * DEG2RAD;
                break;
            case 2:
                if(rand)
                    newyaw = idealyaw + 90 * DEG2RAD;
                else
                    newyaw = idealyaw - 90 * DEG2RAD;
                break;
            case 3:
                if(rand)
                    newyaw = idealyaw - 135 * DEG2RAD;
                else
                    newyaw = idealyaw + 135 * DEG2RAD;
                break;
            case 4:
                if(rand)
                    newyaw = idealyaw + 135 * DEG2RAD;
                else
                    newyaw = idealyaw - 135 * DEG2RAD;
                break;
            case 5:
                newyaw = idealyaw + 180 * DEG2RAD;
                break;
        }
                
        newv.x = cosf(newyaw);
        newv.y = sinf(newyaw);
        newv.z = 0;
    }
    
    while(newyaw > M_PI * 2)
        newyaw -= M_PI * 2;
    while(newyaw < 0)
        newyaw += M_PI * 2;
    
    idealyaw = newyaw;
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
    
    hitplane.vNormal = NormalizeVector3(v * -1.0);
    
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
