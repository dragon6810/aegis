#include "Csg.h"

#include <utilslib.h>

std::string objout = "";

void WriteObjs(void)
{
    int i, j, h, m, f, v;
    model_t *mdl;
    face_t *face;

    FILE *ptr;
    std::string name;
    std::vector<Eigen::Vector3f> verts;
    std::vector<std::vector<int>> index;

    for(h=0; h<Bsplib::n_hulls; h++)
    {
        verts.clear();
        index.clear();

        for(m=0, mdl=models.data(); m<models.size(); m++, mdl++)
        {
            for(f=0; f<mdl->faces[h].size(); f++)
            {
                face = &faces[mdl->faces[h][f]];

                index.push_back({});
                for(v=0; v<face->poly.size(); v++)
                {
                    index.back().push_back(verts.size());
                    verts.push_back(face->poly[v]);
                }
            }
        }

        name = Utilslib::StripExtension(objout.c_str());
        name = name + "_hull_" + std::to_string(h);
        name = Utilslib::AddExtension(name.c_str(), "obj");
        ptr = fopen(name.c_str(), "w");
        if(!ptr)
        {
            fprintf(stderr, "couldn't open file for writing \"%s\".\n", name.c_str());
            exit(1);
        }

        for(i=0; i<verts.size(); i++)
            fprintf(ptr, "v %f %f %f\n", verts[i][0], verts[i][1], verts[i][2]);

        for(i=0; i<index.size(); i++)
        {
            fprintf(ptr, "f ");
            for(j=0; j<index[i].size(); j++)
                fprintf(ptr, "%d ", index[i][j] + 1);
            fprintf(ptr, "\n");
        }

        fclose(ptr);
    }
}

std::vector<face_t> interior, exterior;
int nculled = 0;

void FindExteriorByBrush(brush_t *b1, brush_t *b2, bool b2priority)
{
    int p, f;

    plane_t pl, fpl;
    Mathlib::planeside_e side;

    assert(b1);
    assert(b2);

    for(p=0; p<b2->faces.size(); p++)
    {
        pl = planes[b2->faces[p].planenum];
        if(b2->faces[p].flip)
        {
            pl.n = -pl.n;
            pl.d = -pl.d;
        }

        for(f=0; f<interior.size(); f++)
        {
            side = Mathlib::PolySide(interior[f].poly, pl.n, pl.d);
            switch(side)
            {
            case Mathlib::SIDE_BACK:
                break;
            case Mathlib::SIDE_ON:
                fpl = planes[interior[f].planenum];
                if(interior[f].flip)
                {
                    fpl.n = -fpl.n;
                    fpl.d = -fpl.d;
                }

                if(fpl.n.dot(pl.n) < 0)
                    break;
                
                if(b2priority)
                    break;

            case Mathlib::SIDE_FRONT:
                exterior.push_back(interior[f]);
                interior.erase(interior.begin() + f);
                f--;
                break;
            case Mathlib::SIDE_CROSS:
                exterior.push_back(interior[f]);
                exterior.back().poly = Mathlib::ClipPoly(exterior.back().poly, pl.n, pl.d, Mathlib::SIDE_FRONT);
                interior[f].poly = Mathlib::ClipPoly(interior[f].poly, pl.n, pl.d, Mathlib::SIDE_BACK);
                break;
            default:
                break;
            };
        }
    }
}

void PopulateExteriorBrush(brush_t *br)
{
    int i;

    interior.clear();
    exterior.clear();

    exterior.resize(br->faces.size());
    for(i=0; i<br->faces.size(); i++)
    {
        exterior[i] = {};
        exterior[i].planenum = br->faces[i].planenum;
        exterior[i].flip = br->faces[i].flip;
        exterior[i].nodenum = -1;
        exterior[i].texinfo = br->faces[i].texinfo;
        exterior[i].poly = br->faces[i].poly;
    }
}

void CullInteriorFaces(model_t *mdl)
{
    int i, h, b1, b2, f;

    brush_t *pb1, *pb2;
    int faceoffs;

    for(h=0; h<Bsplib::n_hulls; h++)
    {
        mdl->faces[h].clear();
        for(b1=0, pb1=mdl->brushes[h].data(); b1<mdl->brushes[h].size(); b1++, pb1++)
        {
            PopulateExteriorBrush(pb1);

            for(b2=0, pb2=mdl->brushes[h].data(); b2<mdl->brushes[h].size(); b2++, pb2++)
            {
                if(b1 == b2)
                    continue;

                for(i=0; i<3; i++)
                    if(pb1->bb[0][i] > pb2->bb[1][i] || pb1->bb[1][i] < pb2->bb[0][i])
                        break;
                if(i < 3)
                    continue;

                nculled += interior.size();
                interior = exterior;
                exterior.clear();
                FindExteriorByBrush(pb1, pb2, b2 > b1);
            }
        
            mdl->faces[h].reserve(mdl->faces[h].size() + exterior.size());
            faces.reserve(faces.size() + exterior.size());
            for(i=0; i<exterior.size(); i++)
            {
                mdl->faces[h].push_back(faces.size());
                faces.push_back(exterior[i]);
            }
        }
    }
}

void PolygonizeBrush(brush_t *br)
{
    int i, j, k;

    plane_t pl, _pl;

    for(i=0; i<br->faces.size(); i++)
    {
        pl = planes[br->faces[i].planenum];
        
        br->faces[i].poly = Mathlib::FromPlane(pl.n, pl.d);
        if(br->faces[i].flip)
            br->faces[i].poly = Mathlib::FromPlane(-pl.n, -pl.d);
        else
            br->faces[i].poly = Mathlib::FromPlane(pl.n, pl.d);
        for(j=0; j<br->faces.size(); j++)
        {
            if(i == j)
                continue;

            _pl = planes[br->faces[j].planenum];

            if(br->faces[j].flip)
                br->faces[i].poly = Mathlib::ClipPoly(br->faces[i].poly, _pl.n, _pl.d, Mathlib::SIDE_FRONT);
            else
                br->faces[i].poly = Mathlib::ClipPoly(br->faces[i].poly, _pl.n, _pl.d, Mathlib::SIDE_BACK);
        }

        if(!br->faces[i].poly.size())
        {
            fprintf(stderr, "brush face clipped to size 0!\n");
            exit(1);
        }
    }

    for(i=0; i<br->faces.size(); i++)
    {
        for(j=0; j<br->faces[i].poly.size(); j++)
        {
            for(k=0; k<3; k++)
            {
                if((!i && !j) || br->faces[i].poly[j][k] < br->bb[0][k])
                    br->bb[0][k] = br->faces[i].poly[j][k];
                if((!i && !j) || br->faces[i].poly[j][k] > br->bb[1][k])
                    br->bb[1][k] = br->faces[i].poly[j][k];
            }
        }
    }
}

int nexpand = 0;

void ExpandBrush(brush_t *br, Eigen::Vector3f hull[2])
{
    const float epsilon = 0.1;

    int i, j, k;

    Eigen::Vector3f corner;
    plane_t pl, _pl;
    brface_t newface;
    int sizebeforebb;

    assert(br);

    PolygonizeBrush(br);

    for(i=0; i<br->faces.size(); i++)
    {
        pl = planes[br->faces[i].planenum];
        if(br->faces[i].flip)
        {
            pl.n = -pl.n;
            pl.d = -pl.d;
        }

        corner = Eigen::Vector3f::Zero();
        for(j=0; j<3; j++)
        {
            if(pl.n[j] < epsilon)
                corner[j] = hull[0][j];
            if(pl.n[j] > epsilon)
                corner[j] = hull[1][j];
        }

        pl.d += pl.n.dot(corner);
        br->faces[i].planenum = FindPlane(pl.n, pl.d);
        br->faces[i].flip = false;
        if(br->faces[i].planenum < 0)
        {
            br->faces[i].planenum = ~br->faces[i].planenum;
            br->faces[i].flip = true;
        }
    }

    sizebeforebb = br->faces.size();
    br->faces.reserve(sizebeforebb + 6);
    for(i=0; i<3; i++)
    {
        for(j=0; j<2; j++)
        {
            newface = {};
            pl = {};

            pl.n[i] = j * 2 - 1;
            pl.d = (br->bb[j][i] + hull[j][i]) * (j * 2 - 1);

            // it really fucks things up if you add a coplanar face, so dont add it if it exists already
            for(k=0; k<sizebeforebb; k++)
            {
                _pl = planes[br->faces[k].planenum];
                if(br->faces[k].flip)
                {
                    _pl.n = -_pl.n;
                    _pl.d = -_pl.d;
                }

                if((_pl.n - pl.n).squaredNorm() < epsilon * epsilon && fabsf(_pl.d - pl.d) < epsilon)
                    break;
            }

            if(k < sizebeforebb)
                continue;

            newface.planenum = FindPlane(pl.n, pl.d);
            if(newface.planenum < 0)
            {
                newface.planenum = ~newface.planenum;
                newface.flip = true;
            }
            br->faces.push_back(newface);
        }
    }

    PolygonizeBrush(br);

    nexpand++;
}

void CsgModel(model_t *mdl)
{
    int i, h;

    uint64_t expandstartt, expandendt;
    uint64_t cullstartt, cullendt;
    uint64_t startt, endt;

    startt = TIMEMS;

    assert(mdl);

    printf("---- CsgModel ----\n");

    startt = expandstartt = TIMEMS;

    for(h=1; h<Bsplib::n_hulls; h++)
        mdl->brushes[h] = mdl->brushes[0];

    for(h=0; h<Bsplib::n_hulls; h++)
    {
        for(i=0; i<mdl->brushes[h].size(); i++)
            ExpandBrush(&mdl->brushes[h][i], hulls.hulls[h]);
    }

    expandendt = TIMEMS;

    printf("%d brushes expanded in %llums.\n", nexpand, expandendt - expandstartt);

    cullstartt = TIMEMS;

    CullInteriorFaces(mdl);

    cullendt = TIMEMS;
    printf("%d interior faces culled in %llums.\n", nculled, cullendt - cullstartt);

    if(objout.size())
        WriteObjs();

    endt = TIMEMS;
    printf("CsgModel done in %llums.\n", endt - startt);
}