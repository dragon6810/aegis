#include <bsp/bsp.h>

#include <math.h>
#include <string.h>

#include <bspfile.h>
#include <std/assert/assert.h>
#include <std/profiler/profiler.h>

#include <cli/cli.h>

int16_t bsp_loadintofile_findvertex(vec3_t vertex)
{
    const float epsilon = 0.01;

    int i, j;

    for(i=0; i<bspfile_nverts; i++)
    {
        for(j=0; j<3; j++)
            if(fabsf(vertex[j] - bspfile_verts[i][j]) > epsilon)
                break;

        if(j < 3)
            continue;

        return i;
    }

    if(i >= MAX_MAP_VERTS)
        cli_error(true, "max map vertices reached: max is %d.\n", MAX_MAP_VERTS);
    
    VectorCopy(bspfile_verts[i], vertex);
    return bspfile_nverts++;
}

// returns negative if you need to flip
int32_t bsp_loadintofile_findedge(int16_t e[2])
{
    int i;

    for(i=0; i<bspfile_nedges; i++)
    {
        if(bspfile_edges[i].vertex[0] == e[0] && bspfile_edges[i].vertex[1] == e[1])
            return i;
        if(bspfile_edges[i].vertex[1] == e[0] && bspfile_edges[i].vertex[0] == e[1] && i)
            return -i;
    }

    if(i >= MAX_MAP_EDGES)
        cli_error(true, "max map edges reached: max is %d.\n", MAX_MAP_EDGES);
    bspfile_edges[i].vertex[0] = e[0];
    bspfile_edges[i].vertex[1] = e[1];
    return bspfile_nedges++;
}

uint16_t bsp_loadintofile_addface(bsp_face_t* face, int16_t iplane)
{
    int i;

    bspfile_face_t fileface = {};
    int16_t iverts[face->poly->npoints + 1], e[2];
    vec3_t a, b, n;

    if(bspfile_nfaces >= MAX_MAP_FACES)
        cli_error(true, "max map faces reached: max is %d.\n", MAX_MAP_FACES);

    fileface.texinfo = face->texinfo;

    VectorSubtract(a, face->poly->points[1], face->poly->points[0]);
    VectorSubtract(b, face->poly->points[2], face->poly->points[0]);
    VectorCross(n, a, b);

    fileface.firstmarkedge = bspfile_nmarkedges;
    fileface.nmarkedges = face->poly->npoints;

    for(i=0; i<face->poly->npoints; i++)
        iverts[i] = bsp_loadintofile_findvertex(face->poly->points[i]);
    iverts[i] = iverts[0];

    if(bspfile_nmarkedges + face->poly->npoints-1 >= MAX_MAP_MARKEDGES)
        cli_error(true, "max map mark edges reached: max is %d.\n", MAX_MAP_MARKEDGES);

    for(i=0; i<face->poly->npoints; i++)
    {
        e[0] = iverts[i];
        e[1] = iverts[i + 1];
        bspfile_markedges[bspfile_nmarkedges++] = bsp_loadintofile_findedge(e);
    }

    face->fileface = bspfile_nfaces;
    
    memcpy(&bspfile_faces[bspfile_nfaces], &fileface, sizeof(bspfile_face_t));
    return bspfile_nfaces++;
}

uint32_t bsp_loadintofile_findplane(bsp_plane_t* pl)
{
    int i, j;

    bspfile_plane_t *fileplane;

    for(i=0, fileplane=bspfile_planes; i<bspfile_nplanes; i++, fileplane++)
    {
        for(j=0; j<3; j++)
            if(pl->n[j] != fileplane->n[j])
                break;
        if(j < 3)
            continue;
        if(pl->d != fileplane->d)
            continue;

        return i;
    }

    if(bspfile_nplanes >= MAX_MAP_PLANES)
        cli_error(true, "max map planes reached: max is %d.\n", MAX_MAP_PLANES);

    fileplane = &bspfile_planes[bspfile_nplanes];
    VectorCopy(fileplane->n, pl->n);
    fileplane->d = pl->d;
    
    return bspfile_nplanes++;
}

// doesn't set leaf indices, must be done manually
uint16_t bsp_loadintofile_findportal(bsp_portal_t* prt)
{
    int i, j;

    bspfile_portal_t *fileprt;
    int16_t iverts[prt->poly->npoints+1], e[2];

    assert(prt);

    if(prt->fileprt >= 0)
        return prt->fileprt;

    if(bspfile_nportals >= MAX_MAP_PORTALS)
        cli_error(true, "max map portals reached: max is %d.\n", MAX_MAP_PORTALS);

    fileprt = &bspfile_portals[bspfile_nportals];
    fileprt->leaves[0] = fileprt->leaves[1] = -1;
    fileprt->plane = bsp_loadintofile_findplane(prt->pl);

    if(bspfile_nmarkedges + prt->poly->npoints-1 >= MAX_MAP_MARKEDGES)
        cli_error(true, "max map mark edges reached: max is %d.\n", MAX_MAP_MARKEDGES);

    for(i=0; i<prt->poly->npoints; i++)
        iverts[i] = bsp_loadintofile_findvertex(prt->poly->points[i]);
    iverts[i] = iverts[0];

    fileprt->firstmarkedge = bspfile_nmarkedges;
    fileprt->nmarkedges = prt->poly->npoints;
    
    for(i=0; i<prt->poly->npoints; i++)
    {
        for(j=0; j<2; j++)
            e[j] = iverts[i + j];
        bspfile_markedges[bspfile_nmarkedges++] = bsp_loadintofile_findedge(e);
        printf("index vs nmarkedge: %d, %d\n", fileprt->firstmarkedge + i, bspfile_nmarkedges);
    }

    return bspfile_nportals++;
}

int16_t bsp_loadintofile_addclipleaf(bsp_leaf_t* leaf)
{
    int i, j;

    bspfile_clipleaf_t *fileleaf;
    bspfile_portal_t *fileprt;
    bsp_portal_t *prt;

    assert(leaf);

    if(bspfile_nclipleafs >= MAX_MAP_CLIPLEAFS)
        cli_error(true, "max map clipleafs reached: max is %d.\n", MAX_MAP_CLIPLEAFS);

    fileleaf = &bspfile_clipleafs[bspfile_nclipleafs];

    fileleaf->contents = leaf->contents;
    fileleaf->firstmarkportal = bspfile_nmarkportals;
    fileleaf->nmarkportals = leaf->portals.size;

    if(bspfile_nmarkportals + leaf->portals.size - 1 >= MAX_MAP_MARKPORTALS)
        cli_error(true, "max map markportals reached: max is %d.\n", MAX_MAP_MARKPORTALS);

    for(i=0; i<leaf->portals.size; i++)
    {
        prt = &bsp_portals[leaf->hull][leaf->portals.data[i]];
        bspfile_markportals[bspfile_nmarkportals++] = bsp_loadintofile_findportal(prt);
        fileprt = &bspfile_portals[bsp_loadintofile_findportal(prt)];
        for(j=0; j<2; j++)
            if(prt->leafs[j] == leaf)
                fileprt->leaves[j] = bspfile_nclipleafs;
    }

    return bspfile_nclipleafs++;
}

int16_t bsp_loadintofile_addclipnode_r(bsp_plane_t* node)
{
    int i;

    bspfile_clipnode_t *filenode;
    int16_t ifilenode;

    assert(node);

    if(bspfile_nclipnodes >= MAX_MAP_CLIPNODES)
        cli_error(true, "max map clipnodes reached: max is %d.\n", MAX_MAP_CLIPNODES);

    filenode = &bspfile_clipnodes[bspfile_nclipnodes];
    ifilenode = bspfile_nclipnodes++;

    filenode->plane = bsp_loadintofile_findplane(node);
    for(i=0; i<2; i++)
    {
        if(node->children[i] > 0)
            filenode->children[i] = bsp_loadintofile_addclipnode_r(&bsp_planes[node->hull][node->children[i]]);
        else
            filenode->children[i] = ~bsp_loadintofile_addclipleaf(bsp_leaves[node->hull][~node->children[i]]); 
    }

    return ifilenode;
}

int32_t bsp_loadintofile_addleaf(bsp_leaf_t* leaf)
{
    int i;

    bspfile_leaf_t* fileleaf;
    int32_t ileaf;

    if(bspfile_nleafs >= MAX_MAP_LEAFS)
        cli_error(true, "max map leaves reached: max is %d.\n", MAX_MAP_LEAFS);

    ileaf = bspfile_nleafs;
    fileleaf = &bspfile_leafs[bspfile_nleafs++];
    memset(fileleaf, 0, sizeof(bspfile_leaf_t));

    if(bspfile_nmarksurfs + leaf->nfaces >= MAX_MAP_MARKSURFACES)
        cli_error(true, "max map marksurfs reached: max is %d.\n", MAX_MAP_MARKSURFACES);

    fileleaf->firstmarkface = bspfile_nmarksurfs;
    fileleaf->nfaces = leaf->nfaces;
    for(i=0; i<leaf->nfaces; i++)
        bspfile_marksurfs[bspfile_nmarksurfs++] = bsp_faces[leaf->hull][leaf->faces[i]].fileface;

    return ileaf;
}

int32_t bsp_loadintofile_addnode_r(bsp_plane_t* node)
{
    int i;

    bspfile_node_t* filenode;
    int32_t inode;

    if(bspfile_nnodes >= MAX_MAP_NODES)
        cli_error(true, "max map nodes reached: max is %d.\n", MAX_MAP_NODES);

    inode = bspfile_nnodes;
    filenode = &bspfile_nodes[bspfile_nnodes++];
    memset(filenode, 0, sizeof(bspfile_node_t));

    filenode->plane = bsp_loadintofile_findplane(node);
    VectorCopy(filenode->bbox[0], node->bounds[0]);
    VectorCopy(filenode->bbox[1], node->bounds[1]);

    filenode->firstface = bspfile_nfaces;
    filenode->nfaces = node->faces.size;

    for(i=0; i<node->faces.size; i++)
        bsp_loadintofile_addface(&bsp_faces[node->hull][node->faces.data[i]], bspfile_nplanes);

    for(i=0; i<2; i++)
    {
        if(node->children[i] > 0)
            filenode->children[i] = bsp_loadintofile_addnode_r(&bsp_planes[node->hull][node->children[i]]);
        else
            filenode->children[i] = ~bsp_loadintofile_addleaf(bsp_leaves[node->hull][~node->children[i]]);
    }

    return inode;
}

void bsp_loadintofile_addmodel(int imodel)
{
    int i;

    bspfile_model_t* filemodel;

    if(bspfile_nmodels >= MAX_MAP_MODELS)
        cli_error(true, "max map models reached: max is %d.\n", MAX_MAP_MODELS);

    filemodel = &bspfile_models[bspfile_nmodels++];
    memset(filemodel, 0, sizeof(bspfile_model_t));

    filemodel->index = imodel;
    for(i=0; i<MAX_MAP_HULLS; i++)
    {
        filemodel->firstportal[i] = bspfile_nportals;
        if(!i)
            filemodel->renderhead = bsp_loadintofile_addnode_r(&bsp_planes[i][bsp_models[imodel].headplane[i]]);
        filemodel->headnodes[i] = bsp_loadintofile_addclipnode_r(&bsp_planes[i][bsp_models[imodel].headplane[i]]);
        filemodel->nportals[i] = bspfile_nportals - filemodel->firstportal[i];
    }
}

void bsp_loadintofile_copyents(void)
{
    strcpy(bspfile_entities, bsp_entstring);
}

int16_t bsp_loadintofile_addtexinfo(bsp_texinfo_t* texinfo)
{
    int i;

    int16_t itexinfo;
    bspfile_texinfo_t *filetexinfo;

    assert(texinfo);

    if(bspfile_ntexinfo >= MAX_MAP_TEXINFO)
        cli_error(true, "max map texinfo reached: max is %d.\n", MAX_MAP_TEXINFO);

    itexinfo = bspfile_ntexinfo;
    filetexinfo = &bspfile_texinfo[bspfile_ntexinfo++];
    memset(filetexinfo, 0, sizeof(bspfile_texinfo_t));

    for(i=0; i<2; i++)
    {
        VectorCopy(filetexinfo->basis[i], texinfo->basis[i]);
        filetexinfo->shift[i] = texinfo->shift[i];
    }

    strcpy(filetexinfo->miptex, texinfo->texname);

    return itexinfo;
}

void bsp_loadintofile(void)
{
    int i;

    profiler_push("Load Into File");

    bsp_loadintofile_copyents();
    for(i=0; i<bsp_ntexinfos; i++)
        bsp_loadintofile_addtexinfo(&bsp_texinfos[i]);
    for(i=0; i<bsp_nmodels; i++)
        bsp_loadintofile_addmodel(i);

    profiler_pop();
}