#include "PolyMath.h"

#include <cassert>
#include <math.h>

#include <mathlib.h>

#include "Console.h"

std::vector<Eigen::Vector3f> PolyMath::BaseWindingForPlane(Eigen::Vector3f n, float d, float maxrange)
{
	const float epsilon = 0.01;

    int i;

    int axis;
    float curaxis, maxaxis;
    Eigen::Vector3f up, right, origin;
	std::vector<Eigen::Vector3f> winding;

    axis = -1;
    maxaxis = 0;
    for(i=0; i<3; i++)
    {
        curaxis = fabsf(n[i]);
        if(curaxis > maxaxis)
        {
            axis = i;
            maxaxis = curaxis;
        }
    }
    if(axis < 0)
        return {};

    up = right = Eigen::Vector3f();
    switch(axis)
    {
    case 0:
        up[2] = 1;
        break;
    case 1:
        up[2] = 1;
        break;
    case 2:
        up[0] = 1;
        break;
    default:
        break;
    }

	up = up - n * up.dot(n);
    up.normalize();
	assert(!(up == n));

    right = up.cross(n);

	origin = n * d;
	up = up * maxrange;
	right = right * maxrange;

	winding.resize(4);
	winding[0] = origin + up + right;
	winding[1] = origin + up - right;
	winding[2] = origin - up - right;
	winding[3] = origin - up + right;

	assert(fabsf(winding[0].dot(n) - d) < epsilon);
	assert(fabsf(winding[1].dot(n) - d) < epsilon);
	assert(fabsf(winding[2].dot(n) - d) < epsilon);
	assert(fabsf(winding[3].dot(n) - d) < epsilon);

	return winding;
}

bool PolyMath::PlaneCrosses(std::vector<Eigen::Vector3f> points, Eigen::Vector3f n, float d)
{
	const float epsilon = 0.01;

	int i;
    
    float d1, d2;

	for(i=0; i<points.size(); i++)
    {
        d1 = points[i].dot(n) - d;
        d2 = points[(i+1)%points.size()].dot(n) - d;

        if(d1 > epsilon && d2 < -epsilon)
            return true;
        if(d1 < -epsilon && d2 > epsilon)
            return true;
    }

	return false;
}

std::vector<Eigen::Vector3f> PolyMath::ClipToPlane(std::vector<Eigen::Vector3f> points, Eigen::Vector3f n, float d, bool front)
{
	const float epsilon = 0.01;

    int i;
    
    int firstout, firstin, firstside;
	int last, cur;
	bool sameside;
    float d1, d2, t;
	std::vector<Eigen::Vector3f> newpoints;

    if(front)
    {
        n = n * -1;
        d = -d;
    }

	firstside = 0;
	sameside = true;
	for(i=0; i<points.size(); i++)
    {
        d1 = points[i].dot(n) - d;
		
		if(firstside == 0)
		{
			if(d1 < -epsilon)
				firstside = -1;
			if(d1 > epsilon)
				firstside = 1;

			continue;
		}
		if(d1 * firstside < -epsilon)
		{
			sameside = false;
			break;
		}
    }

	if(sameside)
	{
		if(firstside < 1)
			return points;

		return {};
	}

	firstout = firstin = -1;
    for(i=0; i<points.size(); i++)
    {
        d1 = points[(i - 1 + points.size()) % points.size()].dot(n) - d;
        d2 = points[i].dot(n) - d;

		if((d1 < epsilon) && (d2 > epsilon))
			firstout = i;
		if((d2 < epsilon) && (d1 > epsilon))
			firstin = i;
    }

	if(firstout == -1 || firstin == -1)
	{
		Console::Print("PolyMath::ClipToPlane: can't find either firstin or firstout (or both).\n");
		abort();
	}

	if(firstout == ((firstin - 1 + points.size()) % points.size()))
	{
		points.insert(points.begin() + firstout, points[firstout]);
		
		// No need to mod since the array has grown
		if(firstin > firstout)
			firstin++;
	}

	last = (firstout - 1 + points.size()) % points.size();
	cur = firstout;
	d1 = points[last].dot(n) - d;
	d2 = points[cur].dot(n) - d;
	
	assert(d1 - d2 != 0);

	t = -d1 / (d2 - d1);
    points[cur] = LERP(points[last], points[cur], t);

	cur = (firstin - 1 + points.size()) % points.size();
	last = firstin;
	d1 = points[last].dot(n) - d;
	d2 = points[cur].dot(n) - d;
	
	assert(d1 - d2 != 0);
	
	t = -d1 / (d2 - d1);
    points[cur] = LERP(points[last], points[cur], t);

	firstin = (firstin - 1 + points.size()) % points.size();
	for(i=0; i<points.size(); i++)
	{
		if(firstout < firstin)
		{
			if((i < firstin) && (i > firstout))
				continue;
		}
		if(firstin < firstout)
		{
			if((i < firstin) || (i > firstout))
				continue;
		}

		newpoints.push_back(points[i]);
	}

    return newpoints;
}

Eigen::Vector3f PolyMath::FindCenter(std::vector<Eigen::Vector3f> points)
{
    int i;

    Eigen::Vector3f center;

    center = Eigen::Vector3f(0, 0, 0);
    for(i=0; i<points.size(); i++)
        center = center + points[i];
    center = center / points.size();

    return center;
}

Eigen::Vector3f PolyMath::FindNormal(std::vector<Eigen::Vector3f> points)
{
    Eigen::Vector3f a, b, n;

    if(points.size() < 3)
        return Eigen::Vector3f(0, 0, 0);

    a = points[1] - points[0];
    b = points[2] - points[0];
    n = a.cross(b);
    n.normalize();

    return n;
}

std::optional<Eigen::Vector3f> PolyMath::SegmentIntersects(std::vector<Eigen::Vector3f> points, Eigen::Vector3f a, Eigen::Vector3f b)
{
    int i;

    Eigen::Vector3f n;
    std::optional<Eigen::Vector3f> intersect;
    Eigen::Matrix3f toplane;
    std::vector<Eigen::Vector3f> polyproj3;
    std::vector<Eigen::Vector2f> polyproj;
    std::optional<Eigen::Vector3f> planeinter;
    Eigen::Vector3f inter3, interproj3;
    Eigen::Vector2f interproj;

    intersect = std::optional<Eigen::Vector3f>();
    if(points.size() < 3)
        return intersect;

    n = FindNormal(points);
    planeinter = SegmentPlane(n, points[0].dot(n), a, b);
    if(!planeinter.has_value())
        return intersect;

    inter3 = planeinter.value();

    toplane = PlaneProjection(n);
    interproj3 = toplane * inter3;
    interproj = interproj3.head<2>();
    polyproj3.resize(points.size());
    polyproj.resize(points.size());
    for(i=0; i<polyproj3.size(); i++)
    {
        polyproj3[i] = toplane * points[i];
        polyproj[i] = polyproj3[i].head<2>();
    }

    if(!PointIn2d(polyproj, interproj))
        return intersect;
    
    intersect = inter3;
    return intersect;
}

Eigen::Matrix3f PolyMath::PlaneProjection(Eigen::Vector3f n)
{
    Eigen::Vector3f x, y, z;
    Eigen::Matrix3f mat;

    y = Eigen::Vector3f(0, 0, 1);
    if(y == n)
        y = Eigen::Vector3f(0, 1, 0);
    x = y.cross(n);
    y = x.cross(n);
    z = x.cross(y);

    mat.col(0) = x;
    mat.col(1) = y;
    mat.col(2) = z;
    mat.transposeInPlace();

    return mat;
}

std::optional<Eigen::Vector3f> PolyMath::SegmentPlane(Eigen::Vector3f n, float d, Eigen::Vector3f a, Eigen::Vector3f b)
{
    std::optional<Eigen::Vector3f> intersect;
    Eigen::Vector3f r;
    float numer, denom, t, maxt;

    intersect = std::optional<Eigen::Vector3f>();

    r = b - a;
    maxt = r.norm();
    r = r / maxt;

    denom = -r.dot(n);
    if(denom == 0)
        return intersect;
    numer = a.dot(n) - d;
    t = numer / denom;
    if(t < 0 || t > maxt)
        return intersect;

    intersect = a + r * t;
    return intersect;
}

bool PolyMath::PointIn2d(std::vector<Eigen::Vector2f> points, Eigen::Vector2f p)
{
    int i;

    int nintersects;
    float invslope;
    Eigen::Vector2f *curedge[2];
    Eigen::Vector2f intersect;

    if(points.size() < 3)
        return false;

    for(i=nintersects=0; i<points.size(); i++)
    {
        curedge[0] = &points[i];
        curedge[1] = &points[(i+1)%points.size()];

        if((*curedge[0])[1] == (*curedge[1])[1])
            continue;
        if((*curedge[0])[0] < p[0] && (*curedge[1])[0] < p[0])
            continue;
        if(((*curedge[0])[1] - p[1]) * ((*curedge[1])[1] - p[1]) > 0)
            continue;
        invslope = ((*curedge[1])[0] - (*curedge[0])[0]) / ((*curedge[1])[1] - (*curedge[0])[1]);
        intersect = *curedge[0] + Eigen::Vector2f(invslope * (p[1] - (*curedge[0])[1]), p[1] - (*curedge[0])[1]);
        if(intersect[0] < p[0])
            continue;

        nintersects++;
    }

    return nintersects & 1;
}