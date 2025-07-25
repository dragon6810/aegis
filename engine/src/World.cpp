#include "World.h"

#include <math.h>
#include <string.h>
#include <cassert>

#include "PolyMath.h"

#include "Command.h"
#include "Console.h"

#include "EntityCamera.h"
#include "EntityPlayer.h"
#include "EntityZombie.h"

// Entity factory, update with new entity classnames
std::unordered_map<std::string, std::function<std::shared_ptr<EntityBase>()>> entityfactory =
{
	{"player_camera", []() { return std::make_shared<EntityCamera>(); }},
	{"player_tank", []() { return std::make_shared<EntityPlayer>(); }},
	{"zombie_grunt", []() { return std::make_shared<EntityZombie>(); }},
};

std::array<std::array<Eigen::Vector3f, 2>, 4> World::hulls =
{
    {
        { Eigen::Vector3f(  0,   0,   0), Eigen::Vector3f(  0,   0,   0) },
        { Eigen::Vector3f(-16, -16, -36), Eigen::Vector3f( 16,  16,  36) },
        { Eigen::Vector3f(-32, -32, -32), Eigen::Vector3f( 32,  32,  32) },
        { Eigen::Vector3f(-16, -16, -18), Eigen::Vector3f( 16,  16,  18) },
    }
};

bool World::TraceDir_R(int icurnode, traceresult_t* trace, Eigen::Vector3f start, Eigen::Vector3f end, Eigen::Vector3f n)
{
	const float epsilon = 0.01;

	hullnode_t *curnode;
	hullleaf_t *curleaf;
	Eigen::Vector3f cross;
	float d1, d2, t;
	int first;

	if(icurnode < 0)
	{
		curleaf = &this->clipleafs[~icurnode];
		if(curleaf->contents == CONTENTS_SOLID)
		{
			trace->hit = start;
			trace->n = n;
			return true;
		}

		return false;
	}

	curnode = &clipnodes[icurnode];

	d1 = start.dot(planes[curnode->pl].n) - planes[curnode->pl].d;
	d2 = end.dot(planes[curnode->pl].n) - planes[curnode->pl].d;

	if(fabsf(d1) < epsilon && fabsf(d2) < epsilon)
	{
		if(TraceDir_R(curnode->children[0], trace, start, end, planes[curnode->pl].n))
			return true;
		
		return TraceDir_R(curnode->children[1], trace, start, end, planes[curnode->pl].n);
	}

	if(d1 * d2 > 0)
	{
		if(d1 > 0)
			return TraceDir_R(curnode->children[1], trace, start, end, planes[curnode->pl].n);

		return TraceDir_R(curnode->children[0], trace, start, end, planes[curnode->pl].n);
	}

	t = d1 / (d1 - d2);
	cross = LERP(start, end, t);

	first = d1 > 0;

	if(TraceDir_R(curnode->children[first], trace, start, cross, planes[curnode->pl].n))
		return true;
	
	return TraceDir_R(curnode->children[!first], trace, cross, end, planes[curnode->pl].n);
}

traceresult_t World::TraceDir(int headnode, Eigen::Vector3f start, Eigen::Vector3f end)
{
	traceresult_t trace;

	if(headnode >= clipnodes.size())
	{
		Console::Print("World::TraceDir: headnode out of bounds.\n");
		trace.didhit = false;
		return trace;
	}

	trace.start = start;
	trace.end = end;
	trace.didhit = TraceDir_R(headnode, &trace, start, end, Eigen::Vector3f());
	return trace;
}

int World::GetContents(Eigen::Vector3f pos, int node)
{
	float d;
	hullnode_t *curnode;

	if(node < 0)
		return node;

	curnode = &clipnodes[node];
	d = pos.dot(planes[curnode->pl].n) - planes[curnode->pl].d;

	if(d > 0)
		return GetContents(pos, curnode->children[1]);

	return GetContents(pos, curnode->children[0]);
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
	uint64_t lumpoffs;
	uint64_t lumpsize;

	if(!FindLump(ptr, "ENTITY", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"ENTITY\" lump.\n");
		return;
	}

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

void World::LoadClipleaves(FILE* ptr)
{
	int i, j;

	uint64_t lumpoffs, lumpsize, nmarkprts, nclipleaves;
	
	std::vector<uint16_t> markportals;

	int8_t contents;
	uint16_t nmarkportals;
	uint32_t firstmarkportal;

	if(!FindLump(ptr, "IPORTAL", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"IPORTAL\" lump.\n");
		return;
	}

	markportals.resize(lumpsize / 2);
	fread(markportals.data(), sizeof(uint16_t), markportals.size(), ptr);

	if(!FindLump(ptr, "HULLEAF", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"HULLEAF\" lump.\n");
		return;
	}

	nclipleaves = lumpsize / 7;
	clipleafs.resize(nclipleaves);

	Console::Print("Clipleaf Count: %d.\n", nclipleaves);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0; i<nclipleaves; i++)
	{
		fread(&contents, sizeof(contents), 1, ptr);
		fread(&firstmarkportal, sizeof(firstmarkportal), 1, ptr);
		fread(&nmarkportals, sizeof(nmarkportals), 1, ptr);
		clipleafs[i].contents = contents;

		clipleafs[i].portals.reserve(nmarkportals);
		for(j=firstmarkportal; j<firstmarkportal+nmarkportals; j++)
		{
			if(j < markportals.size() && j >= 0)
				clipleafs[i].portals.emplace_back(markportals[j]);
			else
				Console::Print("World::LoadClipleaves: mark portal index out of bounds.\n");
		}
	}
}

void World::LoadClipnodes(FILE* ptr)
{
	int i, j;
	hullnode_t *curnode;

	uint64_t lumpoffs, lumpsize, nclipnodes;

	unsigned short iplane;
	short children[2];

	if(!FindLump(ptr, "HULLBSP", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"HULLBSP\" lump.\n");
		return;
	}

	nclipnodes = lumpsize / 6;
	clipnodes.resize(nclipnodes);

	Console::Print("Clipnode Count: %d.\n", nclipnodes);

	fseek(ptr, lumpoffs, SEEK_SET);
	clipnodes.resize(nclipnodes);
	for(i=0, curnode=clipnodes.data(); i<nclipnodes; i++, curnode++)
	{
		fread(&iplane, sizeof(iplane), 1, ptr);
		fread(curnode->children, sizeof(curnode->children[0]), 2, ptr);

		curnode->pl = iplane;
	}
}

void World::LoadNodes(FILE* ptr)
{
	int i, j;
	node_t *curnode;

	uint64_t lumpoffs, lumpsize, nnodes;

	uint32_t iplane;
	int32_t children[2];
	uint32_t nsurfs, firstsurf;

	if(!FindLump(ptr, "BSP", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"BSP\" lump.\n");
		return;
	}

	nnodes = lumpsize / 44;
	nodes.resize(nnodes);

	Console::Print("Node Count: %d.\n", nnodes);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curnode=nodes.data(); i<nnodes; i++, curnode++)
	{
		fread(&iplane, sizeof(iplane), 1, ptr);
		curnode->pl = iplane;
		for(j=0; j<2; j++)
		{
			fread(&children[j], sizeof(children[j]), 1, ptr);
			curnode->leaves[j] = -1;
			if(children[j] > 0)
				curnode->children[j] = children[j];
			else
				curnode->leaves[j] = ~children[j];
		}

		for(j=0; j<2; j++)
			fread(&curnode->bounds[j], sizeof(float), 3, ptr);

		fread(&nsurfs, sizeof(nsurfs), 1, ptr);
		fread(&firstsurf, sizeof(firstsurf), 1, ptr);
		curnode->surfs.resize(nsurfs);
		for(j=0; j<nsurfs; j++)
			curnode->surfs[j] = j + firstsurf;
	}
}

void World::LoadLeafs(FILE* ptr)
{
	int i, j;
	leaf_t *curleaf;

	uint64_t lumpoffs, lumpsize, nleafs;
	std::vector<uint16_t> marksurfs;
	uint16_t nsurfs, clip;
	uint32_t firstmarksurf;

	if(!FindLump(ptr, "MRKFACE", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"MRKFACE\" lump.\n");
		return;
	}

	marksurfs.resize(lumpsize / 2);
	fread(marksurfs.data(), sizeof(uint16_t), marksurfs.size(), ptr);

	if(!FindLump(ptr, "LEAF", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"LEAF\" lump.\n");
		return;
	}

	nleafs = lumpsize / 32;
	leafs.resize(nleafs);

	Console::Print("Leaf Count: %d.\n", nleafs);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curleaf=leafs.data(); i<nleafs; i++, curleaf++)
	{
		for(j=0; j<2; j++)
			fread(&curleaf->bounds[j], sizeof(float), 3, ptr);
		fread(&firstmarksurf, sizeof(firstmarksurf), 1, ptr);
		fread(&nsurfs, sizeof(nsurfs), 1, ptr);
		curleaf->surfs.reserve(nsurfs);
		for(j=firstmarksurf; j<firstmarksurf+nsurfs; j++)
			curleaf->surfs.emplace_back(marksurfs[j]);
		fread(&clip, sizeof(clip), 1, ptr);
		curleaf->clipleaf = clip;
	}
}

void World::LoadSurfs(FILE* ptr)
{
	int i, j;
	surf_t *cursurf;

	uint64_t lumpoffs, lumpsize, nsurfs;

	int misc;
	long long before;
	int firstedge;
	short nedges;
	unsigned short itex;
	uint32_t firstmarkedge;
    uint16_t nmarkedges;
	uint16_t texinfo;
	int32_t lights[4];

	if(!FindLump(ptr, "FACE", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"FACE\" lump.\n");
		return;
	}

	nsurfs = lumpsize / 24;
	surfs.resize(nsurfs);
	
	Console::Print("Surf Count: %d.\n", nsurfs);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, cursurf=surfs.data(); i<nsurfs; i++, cursurf++)
	{
		fread(&firstmarkedge, sizeof(firstmarkedge), 1, ptr);
		fread(&nmarkedges, sizeof(nmarkedges), 1, ptr);
		fread(&texinfo, sizeof(texinfo), 1, ptr);
		fread(lights, sizeof(int32_t), 4, ptr);

		cursurf->vertices = LoadFromMarkEdges(ptr, firstmarkedge, nmarkedges);
		cursurf->tex = texinfo;
	}
}

void World::LoadPorts(FILE* ptr)
{
	int i, j;

	uint64_t lumpoffs, lumpsize, nports;
	portal_t *curprt;
	uint32_t firstmarkedge;
	uint16_t nmarkedges;
	int16_t iplane;
	int32_t leaves[2];

	if(!FindLump(ptr, "PORTAL", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"PORTAL\" lump.\n");
		return;
	}

	nports = lumpsize / 16;
	ports.resize(nports);
	
	Console::Print("Portal Count: %d.\n", nports);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0; i<nports; i++)
	{
		curprt = &ports[i];
		fread(&firstmarkedge, sizeof(firstmarkedge), 1, ptr);
		fread(&nmarkedges, sizeof(nmarkedges), 1, ptr);
		fread(&iplane, sizeof(iplane), 1, ptr);
		fread(leaves, sizeof(int32_t), 2, ptr);

		curprt->vertices = LoadFromMarkEdges(ptr, firstmarkedge, nmarkedges);

		curprt->pl = iplane;
		curprt->reverse = false;
		if(curprt->pl < 0)
		{
			curprt->pl = ~iplane;
			curprt->reverse = true;
		}
		curprt->leaves[0] = leaves[0];
		curprt->leaves[1] = leaves[1];
	}
}

std::vector<int> World::LoadFromMarkEdges(FILE *ptr, int firstmarkedge, int nmarkedges)
{
	int i;

	uint64_t before;
	uint64_t lumpoffs, lumpsize, nedge, nmarkedge;
	std::vector<std::array<int16_t, 2>> edges;
	std::vector<int32_t> markedges;
	int32_t markedge;
	std::vector<int> points;

	assert(ptr);

	before = ftell(ptr);

	if(!FindLump(ptr, "EDGE", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"EDGE\" lump.\n");
		fseek(ptr, before, SEEK_SET);
		return std::vector<int>();
	}
	nedge = lumpsize / 4;
	edges.resize(nedge);
	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0; i<nedge; i++)
		fread(edges[i].data(), sizeof(int16_t), 2, ptr);

	if(!FindLump(ptr, "MRKEDGE", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"MRKEDGE\" lump.\n");
		fseek(ptr, before, SEEK_SET);
		return std::vector<int>();
	}
	nmarkedge = lumpsize / 4;
	markedges.resize(nmarkedge);
	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0; i<nmarkedge; i++)
		fread(&markedges[i], sizeof(int32_t), 1, ptr);

	fseek(ptr, before, SEEK_SET);

	if(firstmarkedge < 0 || firstmarkedge + nmarkedges - 1 >= nmarkedge)
	{
		Console::Print("World::LoadFromMarkEdges: mark edge index out of bounds.\n");
		return std::vector<int>();
	}

	for(i=firstmarkedge; i<firstmarkedge+nmarkedges; i++)
	{
		markedge = markedges[i];
		if(markedge >= 0)
		{
			if(markedge >= nedge)
			{
				Console::Print("World::LoadFromMarkEdges: edge index out of bounds.\n");
				continue;
			}

			if(edges[markedge][0] >= this->verts.size())
			{
				Console::Print("World::LoadFromMarkEdges: vertex index out of bounds.\n");
				continue;
			}
			points.push_back(edges[markedge][0]);
		}
		else
		{
			if(-markedge >= nedge)
			{
				Console::Print("World::LoadFromMarkEdges: edge index out of bounds.\n");
				continue;
			}

			if(edges[-markedge][1] >= this->verts.size())
			{
				Console::Print("World::LoadFromMarkEdges: vertex index out of bounds.\n");
				continue;
			}
			points.push_back(edges[-markedge][1]);
		}
	}

	return points;
}

ResourceManager::texture_t* World::LoadTexture(const char* name)
{
	int i;

	ResourceManager::texture_t *tex;

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
	std::unordered_map<std::string, ResourceManager::texture_t*> foundmiptex;

	uint64_t lumpoffs;
	uint64_t lumpsize;
	uint64_t ntexinfo;
	char miptex[16];

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

	if(!FindLump(ptr, "TEXINFO", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"TEXINFO\" lump.\n");
		return;
	}

	ntexinfo = lumpsize / 48;
	texinfos.resize(ntexinfo);

	Console::Print("Tex Info Count: %d.\n", ntexinfo);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curtex=texinfos.data(); i<ntexinfo; i++, curtex++)
	{
		fread(&curtex->s, sizeof(float), 3, ptr);
		fread(&curtex->t, sizeof(float), 3, ptr);
		fread(&curtex->sshift, sizeof(float), 1, ptr);
		fread(&curtex->tshift, sizeof(float), 1, ptr);

		fread(miptex, 1, 16, ptr);
		if(foundmiptex.find(miptex) != foundmiptex.end())
		{
			curtex->tex = foundmiptex.at(miptex);
			continue;
		}

		curtex->tex = LoadTexture(miptex);
		foundmiptex[miptex] = curtex->tex;
	}
}

void World::LoadVerts(FILE* ptr)
{
	int i;
	Eigen::Vector3f *curvert;

	uint64_t lumpoffs;
	uint64_t lumpsize;
	uint64_t nverts;

	if(!FindLump(ptr, "VERTEX", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"VERTEX\" lump.\n");
		return;
	}

	nverts = lumpsize / 12;
	verts.resize(nverts);

	Console::Print("Vertex Count: %d.\n", nverts);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curvert=verts.data(); i<nverts; i++, curvert++)
	{
		fread(&(*curvert)[0], sizeof(float), 1, ptr);
		fread(&(*curvert)[1], sizeof(float), 1, ptr);
		fread(&(*curvert)[2], sizeof(float), 1, ptr);
	}
}

void World::LoadPlanes(FILE* ptr)
{
	int i;
	plane_t *curplane;

	uint64_t lumpoffs;
	uint64_t lumpsize;
	uint64_t nplanes;

	if(!FindLump(ptr, "PLANE", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"PLANE\" lump.\n");
		return;
	}

	nplanes = lumpsize / 16;
	planes.resize(nplanes);

	Console::Print("Plane Count: %d.\n", nplanes);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0, curplane=planes.data(); i<nplanes; i++, curplane++)
	{
		fread(&curplane->n[0], sizeof(float), 1, ptr);
		fread(&curplane->n[1], sizeof(float), 1, ptr);
		fread(&curplane->n[2], sizeof(float), 1, ptr);
		fread(&curplane->d, sizeof(float), 1, ptr);
	}
}

void World::LoadModels(FILE* ptr)
{
	int i, j, p;

	uint64_t lumpoffs;
	uint64_t lumpsize;
	uint64_t nmodels;
	uint32_t index, renderhead, headnodes[NHULLS];
	uint16_t nportals[NHULLS], firstportal[NHULLS];
	model_t curmdl;

	if(!FindLump(ptr, "MODEL", &lumpoffs, &lumpsize))
	{
		Console::Print("Map has no \"MODEL\" lump.\n");
		return;
	}

	nmodels = lumpsize / 40;

	Console::Print("Model Count: %d.\n", nmodels);

	fseek(ptr, lumpoffs, SEEK_SET);
	for(i=0; i<nmodels; i++)
	{
		fread(&index, sizeof(index), 1, ptr);
		fread(&renderhead, sizeof(renderhead), 1, ptr);
		fread(headnodes, sizeof(headnodes[0]), NHULLS, ptr);
		fread(nportals, sizeof(nportals[0]), NHULLS, ptr);
		fread(firstportal, sizeof(firstportal[0]), NHULLS, ptr);

		curmdl = {};
		curmdl.renderhead = renderhead;
		for(j=0; j<NHULLS; j++)
		{
			curmdl.clipheads[j] = headnodes[j];
			curmdl.portals[j].resize(nportals[j]);
			for(p=0; p<nportals[j]; p++)
				curmdl.portals[j][p] = p + firstportal[j];
			
			this->models[index] = curmdl;
		}
	}
}

bool World::FindLump(FILE* ptr, const char* tag, uint64_t* outloc, uint64_t* outlen)
{
	int i;

	char curtag[9];
	uint64_t loc, len;
	uint64_t before, tableloc;
	uint32_t nlumps;

	assert(ptr);

	before = ftell(ptr);

	fseek(ptr, 8, SEEK_SET);
	fread(&nlumps, sizeof(nlumps), 1, ptr);
	fread(&tableloc, sizeof(tableloc), 1, ptr);

	for(i=0; i<nlumps; i++)
	{
		fseek(ptr, tableloc + i * 24, SEEK_SET);
		fread(curtag, 1, 8, ptr);
		if(strcmp(curtag, tag))
			continue;

		fread(&len, sizeof(len), 1, ptr);
		fread(&loc, sizeof(loc), 1, ptr);
		break;
	}

	fseek(ptr, before, SEEK_SET);
	if(i >= nlumps)
		return false;

	if(outloc)
		*outloc = loc;
	if(outlen)
		*outlen = len;
	return true;
}

bool World::VerifyFile(FILE* ptr)
{
	char magic[5];
	int id;

	if(!ptr)
		return false;

	magic[4] = 0;

	fseek(ptr, 0, SEEK_SET);
	fread(magic, 1, 4, ptr);
	fread(&id, sizeof(int), 1, ptr);

	if(strcmp(magic, "BSP") || id != 31)
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

	LoadModels(ptr);
	LoadEntities(ptr);
	LoadPlanes(ptr);
	LoadVerts(ptr);
	LoadTextures(ptr);
	LoadPorts(ptr);
	LoadSurfs(ptr);
	LoadLeafs(ptr);
	LoadNodes(ptr);
	LoadClipnodes(ptr);
	LoadClipleaves(ptr);

	navmesh.Initialize(this);

	Console::Print("Finished loading map \"%s\".\n", realpath.c_str());

	trace = TraceDir(0, Eigen::Vector3f(0, 0, 128), Eigen::Vector3f(0, 0, -128));

	fclose(ptr);
	return true;
}

void World::RenderSurf(surf_t* surf)
{
	int i;
	Eigen::Vector3f pos;
	texinfo_t *tex;

	tex = &texinfos[surf->tex];

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	if(tex->tex)
		glBindTexture(GL_TEXTURE_2D, tex->tex->name);
	glBegin(GL_POLYGON);
	for(i=0; i<surf->vertices.size(); i++)
	{
		pos = verts[surf->vertices[i]];
		if(tex->tex)
			glTexCoord2f
			(
				(pos.dot(tex->s) + tex->sshift) / tex->tex->width, 
				(pos.dot(tex->t) + tex->tshift) / tex->tex->height
			);
		glVertex3f(pos[0], pos[1], pos[2]);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void World::Render(void)
{
	int i;
	Eigen::Vector3f d, p;

	camera->SetUpGL();
    
    glPointSize(15.0);
	d = camera->mousedir * 4096.0;
	p = TraceDir(0, camera->pos, d + camera->pos).hit;

    glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	glVertex3f(p[0], p[1], p[2]);
	glVertex3f(0, 0, 0);
	glEnd();
	glColor3f(1, 1, 1);

	for(i=0; i<surfs.size(); i++)
		RenderSurf(&surfs[i]);

    for(i=0; i<entities.size(); i++)
    {
        if(entities[i])
            entities[i]->Render();
    }

	navmesh.Render();
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
