#include "Polygon.h"

void Polygon::Clear(void)
{
    this->points.clear();
}

void Polygon::FromPlane(Eigen::Vector3f n, float d, float radius)
{
    int i;

    int axis;
    float bestlength;
    Eigen::Vector3f basis[3]; // forward, right, up
    bool flip;
    Eigen::Vector3f p;
    float t;

    for(i=0; i<3; i++)
    {
        if(!i || fabsf(n[i]) > bestlength)
        {
            axis = i;
            bestlength = fabsf(n[i]);
            flip = false;
            if(n[i] < 0)
                flip = true;
        }
    }

    for(i=0; i<3; i++)
        basis[i] = Eigen::Vector3f(0, 0, 0);
    basis[0][axis] = 1;
    switch(axis)
    {
    case 0:
        basis[1][1] = 1;
        basis[2][2] = 1;
        break;
    case 1:
        basis[1][0] = -1;
        basis[2][2] = 1;
        break;
    case 2:
        basis[1][0] = 1;
        basis[2][1] = 1;
        break;
    }

    if(flip)
    {
        basis[0] *= -1;
        basis[1] *= -1;
    }

    if(n.dot(basis[0]) == 0)
        return;

    points.resize(4);
    for(i=0; i<4; i++)
    {
        p = Eigen::Vector3f(0, 0, 0);
        switch(i)
        {
        case 0:
            p -= basis[1] * radius;
            p -= basis[2] * radius;
            break;
        case 1:
            p += basis[1] * radius;
            p -= basis[2] * radius;
            break;
        case 2:
            p += basis[1] * radius;
            p += basis[2] * radius;
            break;
        case 3:
            p -= basis[1] * radius;
            p += basis[2] * radius;
            break;
        }

        t = (n.dot(p) - d) / -n.dot(basis[0]);
        p += basis[0] * t;
        points[i] = p;
    }
}

void Polygon::Clip(Eigen::Vector3f n, float d, int side)
{
    const float epsilon = 0.01;
    const int side_back = 0, side_on = 1, side_front = 2;

    int i;

    std::vector<int> pointsides; // 0 is back, 1 is on, 2 is front
    std::vector<Eigen::Vector3f> newpoints;
    float dist, t;
    Eigen::Vector3f dir;

    if(side)
    {
        n = n * -1;
        d = -d;
    }

    pointsides.resize(this->points.size() + 1);
    for(i=0; i<this->points.size(); i++)
    {
        dist = n.dot(this->points[i]) - d;
        if(fabsf(dist) < epsilon)
            pointsides[i] = side_on;
        else if(dist < 0)
            pointsides[i] = side_back;
        else
            pointsides[i] = side_front;
    }
    pointsides[i] = pointsides[0];

    newpoints.clear();
    for(i=0; i<this->points.size(); i++)
    {
        if(pointsides[i] == side_on)
        {
            newpoints.push_back(this->points[i]);
            continue;
        }
        
        if(pointsides[i] == side_back)
            newpoints.push_back(this->points[i]);
        
        if(pointsides[i+1] == side_on)
            continue;
        
        if((pointsides[i] == side_back) == (pointsides[i+1] == side_back))
            continue;

        dir = this->points[(i+1)%this->points.size()] - this->points[i];
        dir.normalize();
        dist = (n.dot(this->points[i]) - d) / -n.dot(dir);
        newpoints.push_back(this->points[i] + dir * dist);
    }

    this->points = newpoints;
}