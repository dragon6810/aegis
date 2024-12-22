#include "World.h"

#include <c_math.h>
#include <c_string.h>
#include <cassert>

#include "Command.h"
#include "Console.h"

#include "EntityCamera.h"
#include "EntityPlayer.h"

// Entity factory, update with new entity classnames
std::unordered_map<std::string, std::function<std::shared_ptr<EntityBase>()>> entityfactory =
{
	{"player_camera", []() { return std::make_shared<EntityCamera>(); }},
	{"player_tank", []() { return std::make_shared<EntityPlayer>(); }},
};

std::array<std::array<Vector3, 2>, 4> World::hulls =
{
    {
        { Vector3(  0,   0,   0), Vector3(  0,   0,   0) },
        { Vector3(-16, -16, -36), Vector3( 16,  16,  36) },
        { Vector3(-32, -32, -32), Vector3( 32,  32,  32) },
        { Vector3(-16, -16, -18), Vector3( 16,  16,  18) },
    }
};

bool World::TraceDir_R(int icurnode, traceresult_t* trace, Vector3 start, Vector3 end, Vector3 n)
{
	const float epsilon = 0.01;

	hullnode_t* curnode;
	Vector3 cross;
	float d1, d2, t;
	int first;

	if(icurnode < 0)
	{
		if(icurnode == CONTENTS_SOLID)
		{
			trace->hit = start;
			trace->n = n;
			return true;
		}

		return false;
	}

	curnode = &clipnodes[icurnode];

	d1 = Vector3::Dot(start, curnode->pl->n) - curnode->pl->d;
	d2 = Vector3::Dot(end, curnode->pl->n) - curnode->pl->d;

	if(fabsf(d1) < epsilon && fabsf(d2) < epsilon)
	{
		if(TraceDir_R(curnode->children[0], trace, start, end, curnode->pl->n))
			return true;
		
		return TraceDir_R(curnode->children[1], trace, start, end, curnode->pl->n);
	}

	if(d1 * d2 > 0)
	{
		if(d1 > 0)
			return TraceDir_R(curnode->children[1], trace, start, end, curnode->pl->n);

		return TraceDir_R(curnode->children[0], trace, start, end, curnode->pl->n);
	}

	t = d1 / (d1 - d2);
	cross = Vector3::Lerp(start, end, t);

	first = d1 > 0;

	if(TraceDir_R(curnode->children[first], trace, start, cross, curnode->pl->n))
		return true;
	
	return TraceDir_R(curnode->children[!first], trace, cross, end, curnode->pl->n);
}

traceresult_t World::TraceDir(int headnode, Vector3 start, Vector3 end)
{
	traceresult_t trace;

	trace.start = start;
	trace.end = end;
	trace.didhit = TraceDir_R(headnode, &trace, start, end, Vector3());
	return trace;
}

std::shared_ptr<EntityBase> World::LoadEntity(const std::unordered_map<std::string, std::string>& pairs)
{
	std::string classname;
	std::shared_ptr<EntityBase> ent;

	if (pairs.find("classname") == pairs.end())
		return NULL;

	classname = pairs.at("classname");

	if(classname == "worldspawn")
	{
		if(pairs.find("wad") != pairs.end())
			wadpath = pairs.at("wad");
	}

	if (entityfactory.find(classname) == entityfactory.end())
		return nullptr;

	ent = entityfactory.at(classname)();
	ent->Init(pairs);

	if (classname == "player_camera")
		camera = std::dynamic_pointer_cast<EntityCamera>(ent);

	return ent;
}

void World::LoadEntities(FILE* ptr)
{
	int i;
	char *c;

	std::string buff;
	std::string curstr;
	std::vector<std::string> entbundles;
	std::vector<std::unordered_map<std::string, std::string>> ents;

	char key[32];
	char val[1024];

	int lumpoffs;
	int lumpsize;

	fseek(ptr, 4 + LUMP_ENTITIES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);

	buff.resize(lumpsize);
	fseek(ptr, lumpoffs, SEEK_SET);
	fread(buff.data(), 1, lumpsize, ptr);

	for(c=buff.data(); c<buff.data() + buff.size(); c++)
	{
		if(*c == '{')
		{
			c++;
			continue;
		}
		if(*c == '}')
		{
			c++;
			entbundles.push_back(curstr);
			curstr.clear();
			continue;
		}

		curstr.push_back(*c);
	}

	ents.resize(entbundles.size());
	for(i=0; i<entbundles.size(); i++)
	{
		curstr.clear();
		for(c=entbundles[i].data(); c<entbundles[i].data() + entbundles[i].size(); c++)
		{
			if(*c == '\n')
			{
				memset(key, 0, sizeof(key));
				memset(val, 0, sizeof(val));
				if (sscanf(curstr.c_str(), "\"%[^\"]\" \"%[^\"]\"", key, val) != 2)
				{
					curstr.clear();
					continue;
				}
				curstr.clear();

				ents[i][std::string(key)] = std::string(val);

				continue;
			}

			curstr.push_back(*c);
		}
	}
	
	entities.resize(ents.size());
	for (i=0; i<ents.size(); i++)
		entities[i] = LoadEntity(ents[i]);
}

std::vector<Vector3> World::ClipToPlane(std::vector<Vector3> points, Vector3 n, float d, int side)
{
	const float epsilon = 0.01;

    int i;
    
    int firstout, firstin, firstside;
	int last, cur;
	bool sameside;
    float d1, d2, t;
	std::vector<Vector3> newpoints;

    if(side)
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
		Console::Print("World::ClipToPlane: can't find either firstin or firstout (or both).\n");
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

std::vector<Vector3> World::BaseWindingForPlane(Vector3 n, float d)
{
    const float maxrange = 8192;
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
	assert(up != n);

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

void World::LoadSurfs_r(std::vector<hullsurf_t> parents, int icurnode, int ihull)
{
    int i, j;

	std::vector<hullsurf_t> newverts;
	std::vector<hullsurf_t> newsurfs;
	hullsurf_t curverts, curnewverts;
	hullnode_t *curnode;
	Vector3 a, b, n;
	float d;
    hullsurf_t newsurf;

	if(icurnode == CONTENTS_SOLID)
		return;

	for(i=0; i<parents.size()-1; i++)
	{
		if(!parents.size())
			break;

		n = parents[parents.size()-1].node->pl->n;
		d = parents[parents.size()-1].node->pl->d;

		parents[i].points = ClipToPlane(parents[i].points, n, d, parents[parents.size()-1].flip);

		if(parents[i].points.size())
			continue;

		parents.erase(parents.begin() + i);
		i--;
	}

	if(icurnode < 0)
	{
		for(i=0; i<parents.size(); i++)
			hullsurfs[ihull].push_back(parents[i]);

		return;
	}

	curnode = &clipnodes[icurnode];
	newsurf.points = BaseWindingForPlane(curnode->pl->n, curnode->pl->d);
	newsurf.node = curnode;

	for(i=0; i<parents.size(); i++)
	{
		n = parents[i].node->pl->n;
		d = parents[i].node->pl->d;

		newsurf.points = ClipToPlane(newsurf.points, n, d, parents[i].flip);
	}

	assert(newsurf.points.size() != 0);

	newsurf.flip = false;
    parents.push_back(newsurf);
    for(i=0; i<2; i++)
    {   
		// Clip to front instead
		if(i)
			parents[parents.size() - 1].flip = true;

        LoadSurfs_r(parents, curnode->children[i], ihull);
    }
}

void World::LoadHullSurfs(FILE* ptr)
{
    int i;

    int lumpoffs, lumpsize;
    int index;

    fseek(ptr, 4 + LUMP_MODELS * 8, SEEK_SET);
    fread(&lumpoffs, sizeof(int), 1, ptr);
    fread(&lumpsize, sizeof(int), 1, ptr);
    fseek(ptr, lumpoffs + 36 + sizeof(int), SEEK_SET);
    for(i=0; i<NHULLS; i++)
    {
        fread(&index, sizeof(int), 1, ptr);
        LoadSurfs_r({}, index, i);
    }
}

void World::LoadClipnodes(FILE* ptr)
{
	int i, j;
	hullnode_t *curnode;

	int lumpoffs;
	int lumpsize;
	int nclipnodes;

	unsigned int iplane;
	short children[2];

	fseek(ptr, 4 + LUMP_CLIPNODES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);
	nclipnodes = lumpsize / 8;
	clipnodes.resize(nclipnodes);

	Console::Print("Clipnode Count: %d.\n", nclipnodes);

	fseek(ptr, lumpoffs, SEEK_SET);
	clipnodes.resize(nclipnodes);
	for(i=0, curnode=clipnodes.data(); i<nclipnodes; i++, curnode++)
	{
		fread(&iplane, sizeof(int), 1, ptr);
		fread(curnode->children, sizeof(short), 2, ptr);

		curnode->pl = &planes[iplane];
	}
}

void World::LoadNodes(FILE* ptr)
{
	int i, j;
	node_t *curnode;

	int lumpoffs;
	int lumpsize;
	int nnodes;

	int misc;
	unsigned int iplane;
	short child;
	unsigned short firstface;
	unsigned short nfaces;

	fseek(ptr, 4 + LUMP_NODES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);
	nnodes = lumpsize / 24;
	nodes.resize(nnodes);

	Console::Print("Node Count: %d.\n", nnodes);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curnode=nodes.data(); i<nnodes; i++, curnode++)
	{
		fread(&iplane, sizeof(int), 1, ptr);
		curnode->pl = &planes[iplane];
		for(j=0; j<2; j++)
		{
			child = 0;
			fread(&child, sizeof(short), 1, ptr);
			if(child >= 0)
				curnode->children[j] = &nodes[child];
			else
				curnode->leaves[j] = &leafs[~child];
		}

		for(j=0; j<3; j++)
		{
			misc = 0;
			fread(&misc, sizeof(short), 1, ptr);
			curnode->min[j] = misc;
		}
		for(j=0; j<3; j++)
		{
			misc = 0;
			fread(&misc, sizeof(short), 1, ptr);
			curnode->max[j] = misc;
		}

		fread(&firstface, sizeof(short), 1, ptr);
		fread(&nfaces, sizeof(short), 1, ptr);
		curnode->surfs.resize(nfaces);
		for(j=0; j<nfaces; j++)
			curnode->surfs[j] = &surfs[j + firstface];
	}
}

void World::LoadLeafs(FILE* ptr)
{
	int i, j;
	leaf_t *curleaf;

	int lumpoffs;
	int lumpsize;
	int nleafs;

	int misc;
	long long before;
	unsigned short firstmsurf, nsurfs;

	fseek(ptr, 4 + LUMP_LEAVES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);
	nleafs = lumpsize / 23;
	leafs.resize(nleafs);

	Console::Print("Leaf Count: %d.\n", nleafs);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curleaf=leafs.data(); i<nleafs; i++, curleaf++)
	{
		fread(&curleaf->contents, sizeof(int), 1, ptr);
		fseek(ptr, 4, SEEK_CUR);

		misc = 0;
		for(j=0; j<3; j++)
		{
			fread(&misc, sizeof(short), 1, ptr);
			curleaf->min[j] = misc;
		}
		for(j=0; j<3; j++)
		{
			fread(&misc, sizeof(short), 1, ptr);
			curleaf->max[j] = misc;
		}

		fread(&firstmsurf, sizeof(short), 1, ptr);
		fread(&nsurfs, sizeof(short), 1, ptr);

		before = ftell(ptr);
		curleaf->surfs.resize(nsurfs);
		misc = 0;
		for(j=0; j<nsurfs; j++)
		{
			fseek(ptr, 4 + LUMP_MARKSURFACES * 8, SEEK_SET);
			fread(&lumpoffs, sizeof(int), 1, ptr);
			fseek(ptr, lumpoffs + (j + firstmsurf) * sizeof(short), SEEK_SET);

			fread(&misc, sizeof(short), 1, ptr);
			curleaf->surfs[j] = &surfs[misc];
		}

		fseek(ptr, before, SEEK_SET);
		fseek(ptr, sizeof(char) * 4, SEEK_CUR);
	}
}

void World::LoadSurfs(FILE* ptr)
{
	int i, j;
	surf_t *cursurf;

	int lumpoffs;
	int lumpsize;
	int nsurfs;

	int misc;
	long long before;
	int firstedge;
	short nedges;
	unsigned short itex;

	fseek(ptr, 4 + LUMP_FACES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);
	nsurfs = lumpsize / 20;
	surfs.resize(nsurfs);

	Console::Print("Surf Count: %d.\n", nsurfs);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, cursurf=surfs.data(); i<nsurfs; i++, cursurf++)
	{
		misc = 0;
		fread(&misc, sizeof(short), 1, ptr);
		cursurf->pl = &planes[misc];
		fread(&misc, sizeof(short), 1, ptr);
		cursurf->reverse = misc;
		fread(&firstedge, sizeof(int), 1, ptr);
		fread(&nedges, sizeof(short), 1, ptr);

		before = ftell(ptr);
		cursurf->vertices.resize(nedges);
        for(j=0; j<nedges; j++)
		{
			fseek(ptr, 4 + LUMP_SURFEDGES * 8, SEEK_SET);
			fread(&lumpoffs, sizeof(int), 1, ptr);
			fseek(ptr, lumpoffs + (j + firstedge) * sizeof(int), SEEK_SET);

			fread(&misc, sizeof(int), 1, ptr);

			fseek(ptr, 4 + LUMP_EDGES * 8, SEEK_SET);
			fread(&lumpoffs, sizeof(int), 1, ptr);
			fseek(ptr, lumpoffs + abs(misc) * sizeof(short) * 2, SEEK_SET);

			if(misc < 0)
				fseek(ptr, sizeof(short), SEEK_CUR);
			
			misc = 0;
			fread(&misc, sizeof(short), 1, ptr);

			cursurf->vertices[j] = &verts[misc];
        }

		fseek(ptr, before, SEEK_SET);
		fread(&itex, sizeof(short), 1, ptr);
		cursurf->tex = &texinfos[itex];

		fseek(ptr, 8, SEEK_CUR);
	}
}

ResourceManager::texture_t* World::LoadTexture(int index, FILE* ptr)
{
	long long before;

	int i;

	int lumpoffs;

	uint32_t ntextures;
	uint32_t textureoffs;

	char name[16];
	ResourceManager::texture_t *tex;

	before = ftell(ptr);

	fseek(ptr, 4 + LUMP_TEXTURES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fseek(ptr, lumpoffs, SEEK_SET);

	fread(&ntextures, sizeof(int), 1, ptr);
	fseek(ptr, index * sizeof(int), SEEK_CUR);
	fread(&textureoffs, sizeof(int), 1, ptr);
	fseek(ptr, lumpoffs + textureoffs, SEEK_SET);

	fread(name, 1, 16, ptr);
	fseek(ptr, before, SEEK_SET);

	for(i=0; i<wads.size(); i++)
	{
		tex = wads[i].LoadTexture(name);
		if(tex)
			return tex;
	}

	return NULL;
}

void World::LoadTextures(FILE* ptr)
{
	int i;
	texinfo_t *curtex;
	
	char* c;
	std::string realpath;
	Wad w;

	int lumpoffs;
	int lumpsize;
	int ntexinfo;

	uint32_t tex;

	for(i=0, c=wadpath.data(); i<wadpath.size(); i++, c++)
	{
		if(!memcmp(c, Command::datadir.data(), Command::datadir.size()))
			break;
	}

	if(i >= wadpath.size())
	{
		Console::Print("Wad not in game dir \"%s\".\n", wadpath.c_str());
		return;
	}

	realpath.resize(wadpath.size() - i - Command::datadir.size());
	strcpy(realpath.data(), c + Command::datadir.size());
	
	w.Open(realpath);
	wads.push_back(w);

	fseek(ptr, 4 + LUMP_TEXINFO * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);
	ntexinfo = lumpsize / 40;
	texinfos.resize(ntexinfo);

	Console::Print("Tex Info Count: %d.\n", ntexinfo);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curtex=texinfos.data(); i<ntexinfo; i++, curtex++)
	{
		fread(&curtex->s, sizeof(float), 3, ptr);
		fread(&curtex->sshift, sizeof(float), 1, ptr);
		fread(&curtex->t, sizeof(float), 3, ptr);
		fread(&curtex->tshift, sizeof(float), 1, ptr);

		fread(&tex, sizeof(int), 1, ptr);
		curtex->tex = LoadTexture(tex, ptr);
		fseek(ptr, sizeof(int), SEEK_CUR);
	}
}

void World::LoadVerts(FILE* ptr)
{
	int i;
	Vector3 *curvert;

	int lumpoffs;
	int lumpsize;
	int nverts;

	fseek(ptr, 4 + LUMP_VERTICES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);
	nverts = lumpsize / 12;
	verts.resize(nverts);

	Console::Print("Vertex Count: %d.\n", nverts);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curvert=verts.data(); i<nverts; i++, curvert++)
	{
		fread(&curvert->x, sizeof(float), 1, ptr);
		fread(&curvert->y, sizeof(float), 1, ptr);
		fread(&curvert->z, sizeof(float), 1, ptr);
	}
}

void World::LoadPlanes(FILE* ptr)
{
	int i;
	plane_t *curplane;

	int lumpoffs;
	int lumpsize;
	int nplanes;

	fseek(ptr, 4 + LUMP_PLANES * 8, SEEK_SET);
	fread(&lumpoffs, sizeof(int), 1, ptr);
	fread(&lumpsize, sizeof(int), 1, ptr);
	nplanes = lumpsize / 20;
	planes.resize(nplanes);

	Console::Print("Plane Count: %d.\n", nplanes);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curplane=planes.data(); i<nplanes; i++, curplane++)
	{
		fread(&curplane->n.x, sizeof(float), 1, ptr);
		fread(&curplane->n.y, sizeof(float), 1, ptr);
		fread(&curplane->n.z, sizeof(float), 1, ptr);
		fread(&curplane->d, sizeof(float), 1, ptr);
		fseek(ptr, sizeof(int), SEEK_CUR);
	}
}

bool World::VerifyFile(FILE* ptr)
{
	int id;

	if(!ptr)
		return false;

	fseek(ptr, 0, SEEK_SET);
	fread(&id, sizeof(int), 1, ptr);

	if(id != 31)
		return false;

	return true;
}

bool World::Load(std::string name)
{
	FILE* ptr;
	std::string realpath;
	traceresult_t trace;

	realpath = Command::datadir + "maps/" + name;
	if(strcmp(&realpath[realpath.size() - 4], ".bsp"))
		realpath.append(".bsp");

	ptr = fopen(realpath.c_str(), "rb");
	if(!ptr)
	{
		Console::Print("Can't find map file \"%s\".\n", realpath.c_str());
		return false;
	}

	Console::Print("Loading map \"%s\".\n", realpath.c_str());

	if(!VerifyFile(ptr))
	{
		Console::Print("Map \"%s\" is not a valid BSP31 file.\n", realpath.c_str());
		return false;
	}

	LoadEntities(ptr);
	LoadPlanes(ptr);
	LoadVerts(ptr);
	LoadTextures(ptr);
	LoadSurfs(ptr);
	LoadLeafs(ptr);
	LoadNodes(ptr);
	LoadClipnodes(ptr);
    LoadHullSurfs(ptr);

	nav.Initialize(this);

	Console::Print("Finished loading map \"%s\".\n", realpath.c_str());

	trace = TraceDir(0, Vector3(0, 0, 128), Vector3(0, 0, -128));

	fclose(ptr);
	return true;
}

void World::RenderSurf(surf_t* surf)
{
	int i;
	Vector3 pos;

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	if(surf->tex->tex)
		glBindTexture(GL_TEXTURE_2D, surf->tex->tex->name);
	glBegin(GL_POLYGON);
	for(i=0; i<surf->vertices.size(); i++)
	{
		pos = *surf->vertices[i];
		if(surf->tex->tex)
			glTexCoord2f(
				(Vector3::Dot(pos, surf->tex->s) + surf->tex->sshift) / surf->tex->tex->width, 
				(Vector3::Dot(pos, surf->tex->t) + surf->tex->tshift) / surf->tex->tex->height);
		glVertex3f(pos.x, pos.y, pos.z);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void World::Render(void)
{
	int i;
	Vector3 d, p;

	camera->SetUpGL();
    
    glPointSize(15.0);
	d = camera->mousedir * 4096.0;
	p = TraceDir(0, camera->pos, d + camera->pos).hit;

    glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	glVertex3f(p.x, p.y, p.z);
	glEnd();
	glColor3f(1, 1, 1);

	//for(i=0; i<surfs.size(); i++)
	//	RenderSurf(&surfs[i]);

    for(i=0; i<entities.size(); i++)
    {
        if(entities[i])
            entities[i]->Render();
    }

	nav.Render();
}

void World::Tick(void)
{
    int i;

    for(i=0; i<entities.size(); i++)
    {
        if(entities[i])
            entities[i]->Tick();
    }
}
