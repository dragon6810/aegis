#include "World.h"

#include "Command.h"
#include "Console.h"

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

World::traceresult_t World::TraceDir(int headnode, Vector3 start, Vector3 end)
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
				key[0] = val[0] = 0;
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
	nclipnodes = lumpsize / 24;
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
	
	w.Open(realpath.c_str());
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

void World::Render()
{
	int i;

	camera->SetUpGL();

	for(i=0; i<surfs.size(); i++)
		RenderSurf(&surfs[i]);
}