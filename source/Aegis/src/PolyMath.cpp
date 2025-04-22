#include "PolyMath.h"

#include <cassert>
#include <math.h>

#include "Console.h"

std::vector<Vector3> PolyMath::BaseWindingForPlane(Vector3 n, float d, float maxrange)
{
	const float epsilon = 0.01;

    int i;

    int axis;
    float curaxis, maxaxis;
    Vector3 up, right, origin;
	std::vector<Vector3> winding;

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

    up = right = Vector3();
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

	up = up - n * Vector3::Dot(up, n);
    up.Normalize();
	assert(!(up == n));

    right = Vector3::Cross(up, n);

	origin = n * d;
	up = up * maxrange;
	right = right * maxrange;

	winding.resize(4);
	winding[0] = origin + up + right;
	winding[1] = origin + up - right;
	winding[2] = origin - up - right;
	winding[3] = origin - up + right;

	assert(fabsf(Vector3::Dot(winding[0], n) - d) < epsilon);
	assert(fabsf(Vector3::Dot(winding[1], n) - d) < epsilon);
	assert(fabsf(Vector3::Dot(winding[2], n) - d) < epsilon);
	assert(fabsf(Vector3::Dot(winding[3], n) - d) < epsilon);

	return winding;
}

bool PolyMath::PlaneCrosses(std::vector<Vector3> points, Vector3 n, float d)
{
	const float epsilon = 0.01;

	int i;

	int firstside;
    float d1;

	firstside = 0;
	for(i=0; i<points.size(); i++)
    {
        d1 = Vector3::Dot(points[i], n) - d;
		
		if(firstside == 0)
		{
			if(d1 < -epsilon)
				firstside = -1;
			if(d1 > epsilon)
				firstside = 1;

			continue;
		}
		if(d1 * firstside < -epsilon)
			return true;
    }

	return false;
}

std::vector<Vector3> PolyMath::ClipToPlane(std::vector<Vector3> points, Vector3 n, float d, bool front)
{
	const float epsilon = 0.01;

    int i;
    
    int firstout, firstin, firstside;
	int last, cur;
	bool sameside;
    float d1, d2, t;
	std::vector<Vector3> newpoints;

    if(front)
    {
        n = n * -1;
        d = -d;
    }

	firstside = 0;
	sameside = true;
	for(i=0; i<points.size(); i++)
    {
        d1 = Vector3::Dot(points[i], n) - d;
		
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
        d1 = Vector3::Dot(points[(i - 1 + points.size()) % points.size()], n) - d;
        d2 = Vector3::Dot(points[i], n) - d;

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
	d1 = Vector3::Dot(points[last], n) - d;
	d2 = Vector3::Dot(points[cur], n) - d;
	
	assert(d1 - d2 != 0);

	t = -d1 / (d2 - d1);
	points[cur] = Vector3::Lerp(points[last], points[cur], t);

	cur = (firstin - 1 + points.size()) % points.size();
	last = firstin;
	d1 = Vector3::Dot(points[last], n) - d;
	d2 = Vector3::Dot(points[cur], n) - d;
	
	assert(d1 - d2 != 0);
	
	t = -d1 / (d2 - d1);
	points[cur] = Vector3::Lerp(points[last], points[cur], t);

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

Vector3 PolyMath::FindCenter(std::vector<Vector3> points)
{
    int i;

    Vector3 center;

    center = Vector3(0, 0, 0);
    for(i=0; i<points.size(); i++)
        center = center + points[i];
    center = center / points.size();

    return center;
}

Vector3 PolyMath::FindNormal(std::vector<Vector3> points)
{
    Vector3 a, b, n;

    if(points.size() < 3)
        return Vector3(0, 0, 0);

    a = points[1] - points[0];
    b = points[2] - points[0];
    n = Vector3::Cross(a, b);
    n.Normalize();

    return n;
}

std::optional<Vector3> PolyMath::SegmentIntersects(std::vector<Vector3> points, Vector3 a, Vector3 b)
{
    int i;

    Vector3 n;
    std::optional<Vector3> intersect;
    Matrix3x3 toplane;
    std::vector<Vector3> polyproj3;
    std::vector<Vector2> polyproj;
    std::optional<Vector3> planeinter;
    Vector3 inter3, interproj3;
    Vector2 interproj;

    intersect = std::optional<Vector3>();
    if(points.size() < 3)
        return intersect;

    n = FindNormal(points);
    planeinter = SegmentPlane(n, Vector3::Dot(points[0], n), a, b);
    if(!planeinter.has_value())
        return intersect;

    inter3 = planeinter.value();

    toplane = PlaneProjection(n);
    interproj3 = toplane * inter3;
    interproj.x = interproj3.x;
    interproj.y = interproj3.y;
    polyproj3.resize(points.size());
    polyproj.resize(points.size());
    for(i=0; i<polyproj3.size(); i++)
    {
        polyproj3[i] = toplane * points[i];
        polyproj[i].x = polyproj3[i].x;
        polyproj[i].y = polyproj3[i].y;
    }

    if(!PointIn2d(polyproj, interproj))
        return intersect;
    
    intersect = inter3;
    return intersect;
}

Matrix3x3 PolyMath::PlaneProjection(Vector3 n)
{
    Vector3 x, y, z;
    Matrix3x3 mat;

    y = Vector3(0, 0, 1);
    if(y == n)
        y = Vector3(0, 1, 0);
    x = Vector3::Cross(y, n);
    y = Vector3::Cross(x, n);
    z = Vector3::Cross(x, y);

    mat.SetColumn(x, 0);
    mat.SetColumn(y, 1);
    mat.SetColumn(z, 2);

    return mat;
}

std::optional<Vector3> PolyMath::SegmentPlane(Vector3 n, float d, Vector3 a, Vector3 b)
{
    std::optional<Vector3> intersect;
    Vector3 p, o, r;
    float numer, denom, t, maxt;

    intersect = std::optional<Vector3>();

    p = n * d;
    o = a - p;
    r = b - a;
    maxt = r.Length();
    r.Normalize();

    denom = Vector3::Dot(r, n);
    if(denom == 0)
        return intersect;
    numer = Vector3::Dot(o, n);
    t = numer / denom;
    if(t < 0 || t > maxt)
        return intersect;

    intersect = a + r * t;
    return intersect;
}

bool PolyMath::PointIn2d(std::vector<Vector2> points, Vector2 p)
{
    int i;

    int nintersects;
    float invslope;
    Vector2 *curedge[2];
    Vector2 intersect;

    if(points.size() < 3)
        return false;

    for(i=nintersects=0; i<points.size(); i++)
    {
        curedge[0] = &points[i];
        curedge[1] = &points[(i+1)%points.size()];

        if(curedge[0]->x < p.x && curedge[1]->x < p.x)
            continue;
        if((curedge[0]->y - p.y) * (curedge[1]->y - p.y) > 0)
            continue;
        invslope = (curedge[1]->x - curedge[0]->x) / (curedge[1]->y - curedge[0]->y);
        intersect = *curedge[0] + Vector2(invslope * (curedge[0]->y - p.y), curedge[0]->y - p.y);
        if(intersect.x < p.x)
            continue;

        nintersects++;
    }

    return nintersects & 1;
}