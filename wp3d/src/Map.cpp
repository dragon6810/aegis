#include "Map.h"

#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <maplib.h>

void Map::SetupFrame(const Viewport& view)
{
    Eigen::Vector3f pos, basis[3];
    Eigen::Quaternionf rot;
    glm::mat4 viewmat, projmat;
    float aspect;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, view.canvassize.x(), view.canvassize.y());

    pos = view.pos;
    view.GetViewBasis(basis);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(view.type == Viewport::FREECAM)
    {
        aspect = (float)view.canvassize.x() / (float)view.canvassize.y();
        projmat = glm::perspective(glm::radians(view.fov), aspect, 1.0f, max_map_size * 2.0f);
    }
    else
    {
        aspect = (float)view.canvassize.x() / (float)view.canvassize.y();
        projmat = glm::ortho(-view.zoom * aspect, view.zoom * aspect, -view.zoom, view.zoom, 1.0f, max_map_size * 2.0f);
    }
    glLoadMatrixf(&projmat[0][0]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    viewmat = glm::lookAt
    (
        glm::vec3(view.pos[0], view.pos[1], view.pos[2]),
        glm::vec3(view.pos.x() + basis[0].x(), view.pos.y() + basis[0].y(), view.pos.z() + basis[0].z()),
        glm::vec3(basis[2].x(), basis[2].y(), basis[2].z())
    );
    glLoadMatrixf(&viewmat[0][0]);
}

void Map::LoadConfig(void)
{
    this->cfg = Cfglib::CfgFile();

    this->cfg.LoadDefault("fgd", "");
    this->cfg.Load(this->cfgpath.c_str());
    if(this->cfg.pairs["fgd"] != "")
        this->LoadFgd();
}

void Map::PanOrtho(Viewport& view, ImGuiKey key)
{
    const float panspeed = 0.25;

    Eigen::Vector3f basis[3];

    assert(view.type != Viewport::FREECAM);

    view.GetViewBasis(basis);
    switch(key)
    {
        case ImGuiKey_W:
            view.pos += basis[2] * panspeed * view.zoom;
            break;
        case ImGuiKey_A:
            view.pos -= basis[1] * panspeed * view.zoom;
            break;
        case ImGuiKey_S:
            view.pos -= basis[2] * panspeed * view.zoom;
            break;
        case ImGuiKey_D:
            view.pos += basis[1] * panspeed * view.zoom;
            break;
        default:
            break;
    }
}

void Map::MoveFreecam(Viewport& view, ImGuiKey key, float deltatime)
{
    const float movespeed = 300;

    Eigen::Vector3f basis[3];
    Eigen::Vector3f move;
    
    assert(view.type == Viewport::FREECAM);

    view.GetViewBasis(basis);
    switch(key)
    {
        case ImGuiKey_W:
            move = basis[0] *  movespeed * deltatime;
            break;
        case ImGuiKey_A:
            move = basis[1] * -movespeed * deltatime;
            break;
        case ImGuiKey_S:
            move = basis[0] * -movespeed * deltatime;
            break;
        case ImGuiKey_D:
            move = basis[1] *  movespeed * deltatime;
            break;
        default:
            return;
    }

    view.pos += move;
}

void Map::LookFreecam(Viewport& view, ImGuiKey key, float deltatime)
{
    const float turnspeed = M_PI;

    assert(view.type == Viewport::FREECAM);

    Eigen::Vector3f addrot;

    addrot = Eigen::Vector3f(0, 0, 0);
    switch(key)
    {
    case ImGuiKey_UpArrow:
        addrot[1] -= turnspeed * deltatime;

        break;
    case ImGuiKey_LeftArrow:
        addrot[2] += turnspeed * deltatime;
        
        break;
    case ImGuiKey_DownArrow:
        addrot[1] += turnspeed * deltatime;
        
        break;
    case ImGuiKey_RightArrow:
        addrot[2] -= turnspeed * deltatime;
        
        break;
    default:
        return;
    }

    view.rot += addrot;
    if(view.rot[1] > M_PI_2)
        view.rot[1] = M_PI_2;
    if(view.rot[1] < -M_PI_2)
        view.rot[1] = -M_PI_2;
}

void Map::FinalizeEntity(void)
{
    Entity ent;
    std::set<int>::iterator it;

    if(!this->placingentity)
        return;

    ent = Entity();
    if(this->fgd.entclasses[Fgdlib::EntityDef::ENTTYPE_POINT].size())
    {
        it = this->fgd.entclasses[Fgdlib::EntityDef::ENTTYPE_POINT].begin();
        std::advance(it, this->workingenttype);
        ent.FillDefaultPairs(&this->fgd.ents[*it], true);
        ent.pairs["classname"] = this->fgd.ents[*it].classname;
        if(ent.pairs.find("origin") != ent.pairs.end())
            ent.pairs["origin"] = 
                std::to_string(this->workingentity[0]) + " " +
                std::to_string(this->workingentity[1]) + " " +
                std::to_string(this->workingentity[2]) + " ";
    }

    this->entities.push_back(ent);
    this->placingentity = false;
    this->selectiontype = SELECT_ENTITY;
    this->SwitchTool(TOOL_SELECT);
    this->entselection.insert(this->entities.size() - 1);
}

void Map::FinalizeBrush(void)
{
    int i, j;

    Eigen::Vector3f bb[2];
    Brush *br;
    Plane *pl;

    // TODO: brush types other than block

    for(i=0; i<3; i++)
        bb[0][i] = bb[1][i] = brushcorners[0][i];
    for(i=0; i<3; i++)
    {
        if(brushcorners[1][i] < bb[0][i])
            bb[0][i] = brushcorners[1][i];
        if(brushcorners[1][i] > bb[1][i])
            bb[1][i] = brushcorners[1][i];
    }

    //printf("bb: ( %f %f %f ), ( %f %f %f )\n", bb[0][0], bb[0][1], bb[0][2], bb[1][0], bb[1][1], bb[1][2]);

    this->entities[0].brushes.push_back({});
    br = &this->entities[0].brushes.back();
    br->planes.resize(6);
    for(i=0; i<3; i++)
    {
        pl = &br->planes[i * 2 + 0];
        *pl = Plane(*this);
        pl->normal = Eigen::Vector3f(0, 0, 0);
        pl->normal[i] = -1;
        pl->d = -bb[0][i];
        pl->DefaultTexBasis();

        pl = &br->planes[i * 2 + 1];
        *pl = Plane(*this);
        pl->normal = Eigen::Vector3f(0, 0, 0);
        pl->normal[i] = 1;
        pl->d = bb[1][i];
        pl->DefaultTexBasis();
    }

    br->MakeFaces();

    nbrushcorners = 0;
}

void Map::FinalizePlane(void)
{
    int i, j;
    Entity *ent;

    Eigen::Vector3f n;
    float d;
    bool pointent;
    std::unordered_set<int> newentselection, newbrselection;
    std::vector<Entity> newents;
    std::vector<Brush> newbrs;

    if(this->ntriplane != 3)
        return;

    n = (this->triplane[1] - this->triplane[0]).cross(this->triplane[2] - this->triplane[0]);
    n.normalize();
    d = n.dot(this->triplane[0]);

    newentselection.clear();
    newents.clear();
    for(i=0; i<this->entities.size(); i++)
    {
        if(this->selectiontype == SELECT_ENTITY && !this->entselection.contains(i))
            continue;

        ent = &this->entities[i];
        pointent = !ent->brushes.size();

        newbrselection.clear();
        newbrs.clear();
        for(j=0; j<ent->brushes.size(); j++)
        {
            if(this->selectiontype == SELECT_BRUSH && !ent->brselection.contains(j))
            {
                newbrs.push_back(ent->brushes[j]);
                continue;
            }

            ent->brushes[j].AddPlane(n, d, *this);
            if(!ent->brushes[j].points.size())
                continue;

            if(ent->brselection.contains(j))
                newbrselection.insert(newbrs.size());
            newbrs.push_back(ent->brushes[j]);
        }
        ent->brselection = newbrselection;
        ent->brushes = newbrs;

        if(ent->brushes.size() || pointent || (ent->pairs.contains("classname") && ent->pairs["classname"] == "worldspawn"))
        {
            if(this->entselection.contains(i))
                this->entselection.insert(newents.size());
            newents.push_back(*ent);
        }
    }
    this->entselection = newentselection;
    this->entities = newents;

    this->ntriplane = 0;
    this->drawingtriplane = false;
}

void Map::ClearSelection(void)
{
    int i, j, k;

    if(this->selectiontype == SELECT_ENTITY)
    {
        this->entselection.clear();
        return;
    }

    for(i=0; i<this->entities.size(); i++)
    {
        if(this->selectiontype == SELECT_BRUSH)
        {
            this->entities[i].brselection.clear();
            return;
        }
        
        for(j=0; j<this->entities[i].brushes.size(); j++)
            this->entities[i].brushes[j].plselection.clear();
    }
}

void Map::MoveVertexPoints(Eigen::Vector3f add)
{
    int i, j, k;
    std::unordered_set<int>::iterator it;

    Entity *ent;
    Brush *br;

    for(i=0; i<this->entities.size(); i++)
    {
        if(this->selectiontype == SELECT_ENTITY && !this->entselection.contains(i))
            continue;

        ent = &this->entities[i];
        for(j=0; j<ent->brushes.size(); j++)
        {
            if(this->selectiontype == SELECT_BRUSH && !ent->brselection.contains(j))
                continue;

            br = &ent->brushes[j];
            for(it=br->pointselection.begin(); it!=br->pointselection.end(); it++)
                br->points[*it] += add;
            br->UpdateGeometryValid();
        }
    }
}

void Map::FinalizeVertexEdit(void)
{
    int i, j, k;

    Entity *ent;

    for(i=0; i<this->entities.size(); i++)
    {
        if(this->selectiontype == SELECT_ENTITY && !this->entselection.contains(i))
            continue;

        ent = &this->entities[i];
        for(j=0; j<ent->brushes.size(); j++)
        {
            if(this->selectiontype == SELECT_BRUSH && !ent->brselection.contains(j))
                continue;

            ent->brushes[j].FinalizeVertexEdit();
        }
    }
}

void Map::DeleteSelected(void)
{
    int i;
    std::unordered_set<int>::iterator it, _it;

    Fgdlib::EntityDef *def;
    std::vector<int> toremove;
    std::unordered_set<int> newselection;
    int newindex;

    if(this->selectiontype == SELECT_ENTITY)
    {
        for(it=this->entselection.begin(); it!=this->entselection.end(); it++)
        {
            if(!*it)
                this->entities[*it].brushes.clear();
            else
                toremove.push_back(*it);
        }
    }
    else
    {
        for(i=0; i<this->entities.size(); i++)
        {
            this->entities[i].DeleteSelected(*this);
            if(!i || this->entities[i].brushes.size())
                continue;

            def = this->entities[i].GetDef(&this->fgd);
            if(!def || def->type != Fgdlib::EntityDef::ENTTYPE_SOLID)
                continue;
            
            toremove.push_back(i);
        }
    }

    std::sort(toremove.begin(), toremove.end());
    for(i=toremove.size()-1; i>=0; i--)
    {
        newselection.clear();
        for(it=this->entselection.begin(); it!=this->entselection.end(); it++)
        {
            if(*it == toremove[i])
                continue;
            if(*it > toremove[i])
                newselection.insert(*it + 1);
            else
                newselection.insert(*it);
        }
        this->entselection = newselection;
        this->entities.erase(this->entities.begin() + toremove[i]);
    }
}

void Map::DrawGrid(const Viewport& view)
{
    const int colcycle = max_grid_level + 1;
    const float minbrightness = 0.25;
    const float maxbrightness = 1.0;

    int i, j, k, l;

    int firstlevel;
    int axis;
    Eigen::Vector3f basis[3], seg[2];
    Eigen::Vector2f camsize;
    float linedistance, brightness;
    float min, max;

    if(view.type == Viewport::FREECAM)
        return;

    firstlevel = log2f(view.zoom / 16.0);
    if(firstlevel < 0)
        firstlevel = 0;
    if(firstlevel < gridlevel)
        firstlevel = gridlevel;
    if(firstlevel > max_grid_level)
        firstlevel = max_grid_level;

    if(firstlevel > max_grid_level)
        return;

    axis = (int) view.type;
    if(view.type == Viewport::FREECAM)
        axis = 2;

    view.GetViewBasis(basis);

    camsize[0] = view.zoom * ((float) view.canvassize[0] / (float) view.canvassize[1]);
    camsize[1] = view.zoom;

    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_LINES);
    for(i=firstlevel; i<=max_grid_level; i++)
    {
        linedistance = 1 << i;
        brightness = 1.0 / (float) colcycle * (float) ((i % (colcycle)) + 1);
        brightness *= (maxbrightness - minbrightness);
        brightness += minbrightness;
        glColor4f(brightness, brightness, brightness, 0.5);
        for(j=0; j<2; j++)
        {
            min = basis[j+1].dot(view.pos - (camsize[j] + linedistance) * (basis[j+1]));
            if(-max_map_size > min)
                min = -max_map_size;
            max = basis[j+1].dot(view.pos + (camsize[j] + linedistance) * (basis[j+1]));
            if(max_map_size < max)
                max = max_map_size;

            min = roundf(min / (float) linedistance) * (float) linedistance;
            max = roundf(max / (float) linedistance) * (float) linedistance;
            for(k=min; k <= max; k += linedistance)
            {
                seg[0] = seg[1] = Eigen::Vector3f(0, 0, 0);
                seg[0] += k * basis[j+1];
                seg[1] += k * basis[j+1];
                seg[0] += -max_map_size * basis[!j+1];
                seg[1] += max_map_size * basis[!j+1];
                for(l=0; l<2; l++)
                    glVertex3f(seg[l][0], seg[l][1], seg[l][2]);
            }
        }
    }
    glEnd();

    glDisable(GL_ALPHA_TEST);

    glColor3f(1, 1, 1);
}

void Map::DrawWorkingEnt(const Viewport& view)
{
    int i;

    Eigen::Vector3i e[2];

    if(!this->placingentity)
        return;

    glColor3f(1, 1, 0);

    glBegin(GL_LINES);
    for(i=0; i<3; i++)
    {
        e[0] = e[1] = workingentity;
        e[0][i] = -max_map_size;
        e[1][i] = max_map_size;

        glVertex3f(e[0][0], e[0][1], e[0][2]);
        glVertex3f(e[1][0], e[1][1], e[1][2]);
    }
    glEnd();

    glColor3f(1, 1, 1);
}

void Map::DrawWorkingBrush(const Viewport& view)
{
    const Eigen::Vector3f cornercolors[2] = { Eigen::Vector3f(1, 0, 0), Eigen::Vector3f(0, 1, 0) };
    const float dashlen = 8;

    int i, j, k;

    Eigen::Vector3i bbox[2];
    Eigen::Vector3i lengths[3];
    Eigen::Vector3i l[2];
    Eigen::Vector3i basis[3]; // forward, right, up

    if(!this->nbrushcorners)
        return;
    
    for(i=0; i<this->nbrushcorners; i++)
    {
        glBegin(GL_POINTS);
        glColor3f(cornercolors[i][0], cornercolors[i][1], cornercolors[i][2]);
        glVertex3f(this->brushcorners[i][0], this->brushcorners[i][1], this->brushcorners[i][2]);
        glEnd();
    }

    if(i < 2)
        return;
    
    glBegin(GL_POINTS);
    glColor3f(0, 1, 0);
    glVertex3f(this->brushcorners[1][0], this->brushcorners[1][1], this->brushcorners[1][2]);
    glEnd();

    bbox[0] = bbox[1] = this->brushcorners[0];
    for(i=0; i<3; i++)
    {
        if(this->brushcorners[1][i] < bbox[0][i])
            bbox[0][i] = this->brushcorners[1][i];
        if(this->brushcorners[1][i] > bbox[1][i])
            bbox[1][i] = this->brushcorners[1][i];
    }

    for(i=0; i<3; i++)
    {
        lengths[i] = Eigen::Vector3i(0, 0, 0);
        lengths[i][i] = bbox[1][i] - bbox[0][i];
    }

    glColor3f(1, 1, 0);

    for(i=0; i<3; i++)
    {
        for(j=0; j<3; j++)
            basis[j] = lengths[(i + j) % 3];

        for(j=0; j<4; j++)
        {
            l[0] = bbox[0];
            for(k=0; k<2; k++)
                if(j & (1 << k))
                    l[0] += basis[k + 1];
            l[1] = l[0] + basis[0];
            DrawDashedLine(l, dashlen);
        }
    }
}

void Map::DrawTriplane(const Viewport& view)
{
    int i, j;

    Eigen::Vector3f n, v[2];
    float d;
    Mathlib::Poly<3> poly;
    Eigen::Vector3f bb[2];

    if(!this->ntriplane)
        return;

    glPointSize(4.0);
    glBegin(GL_POINTS);
    for(i=0; i<this->ntriplane; i++)
    {
        if(this->triplaneselection.contains(i))
            glColor3f(1, 0, 0);
        else
            glColor3f(1, 1, 0);
        glVertex3f(this->triplane[i][0], this->triplane[i][1], this->triplane[i][2]);
    }
    glEnd();

    if(this->ntriplane < 3)
        return;

    n = (this->triplane[1] - this->triplane[0]).cross(this->triplane[2] - this->triplane[0]);
    n.normalize();
    d = n.dot(this->triplane[0]);
    poly = Mathlib::FromPlane(n, d, Map::max_map_size);
    if(view.type == Viewport::FREECAM)
    {
        glColor4f(1, 1, 0, 0.5);
        glEnable(GL_ALPHA_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);

        glBegin(GL_POLYGON);
        for(i=0; i<poly.size(); i++)
            glVertex3f(poly[i][0], poly[i][1], poly[i][2]);
        glEnd();

        glColor4f(1, 1, 1, 1);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glColor3f(1, 1, 0);

        glBegin(GL_LINES);
        for(i=0; i<poly.size(); i++)
        {
            glVertex3f(poly[i][0], poly[i][1], poly[i][2]);
            glVertex3f(poly[(i+1)%poly.size()][0], poly[(i+1)%poly.size()][1], poly[(i+1)%poly.size()][2]);
        }
        glEnd();

        glColor3f(1, 1, 1);
    }

    for(i=0; i<this->ntriplane; i++)
    {
        for(j=0; j<3; j++)
        {
            if(!i || this->triplane[i][j] < bb[0][j])
                bb[0][j] = this->triplane[i][j];
            if(!i || this->triplane[i][j] > bb[1][j])
                bb[1][j] = this->triplane[i][j];
        }
    }
    v[0] = (bb[0] + bb[1]) / 2.0;
    v[1] = v[0] + n * 32.0;

    glBegin(GL_LINES);
    glColor3f(1, 1, 0);

    glVertex3f(v[0][0], v[0][1], v[0][2]);
    glVertex3f(v[1][0], v[1][1], v[1][2]);

    glColor3f(1, 1, 1);
    glEnd();
}

void Map::DrawDashedLine(Eigen::Vector3i l[2], float dashlen)
{
    int i;

    Eigen::Vector3f dir, curpos;
    float maxlen, curlen, seglen;
    Eigen::Vector3f seg[2];

    assert(l);

    for(i=0; i<3; i++)
    {
        dir[i] = l[1][i] - l[0][i];
        curpos[i] = l[0][i];
    }
    maxlen = dir.norm();
    dir = dir.normalized();
    curlen = 0;

    glBegin(GL_LINES);

    while(curlen < maxlen)
    {
        seglen = dashlen;
        if(seglen > maxlen - curlen)
            seglen = maxlen - curlen;

        seg[0] = curpos;
        seg[1] = curpos + dir * seglen;

        for(i=0; i<2; i++)
            glVertex3f(seg[i][0], seg[i][1], seg[i][2]);

        curlen += dashlen * 2;
        curpos += dir * dashlen * 2;
    }

    glEnd();
}

TextureManager::texture_t* Map::GetSelectedTextureID(void)
{
    TextureManager::archive_t *archive;
    std::map<std::string, TextureManager::texture_t>::iterator it;

    if(selectedtexarchive >= this->texmanager.archives.size())
        return NULL;
    
    archive = &this->texmanager.archives[selectedtexarchive];

    if(selectedtex >= archive->textures.size())
        return NULL;

    it = archive->textures.begin();
    std::advance(it, selectedtex);
    return &it->second;
}

void Map::SwitchTool(tooltype_e type)
{
    if(type == this->tool)
        return;

    this->nbrushcorners = 0;
    this->ntriplane = 0;
    this->drawingtriplane = false;
    if(type == TOOL_PLANE && this->selectiontype == SELECT_PLANE)
        this->selectiontype = SELECT_BRUSH;
    this->tool = type;
}

void Map::KeyDown(Viewport& view, ImGuiKey key, float deltatime)
{
    // so that we dont get messed up by tool hotkeys
    if(ImGui::GetIO().KeyShift)
        return;

    switch(key)
    {
    case ImGuiKey_W:
    case ImGuiKey_A:
    case ImGuiKey_S:
    case ImGuiKey_D:
        if(view.type == Viewport::FREECAM)
            MoveFreecam(view, key, deltatime);

        break;
    case ImGuiKey_UpArrow:
    case ImGuiKey_LeftArrow:
    case ImGuiKey_DownArrow:
    case ImGuiKey_RightArrow:
        if(view.type == Viewport::FREECAM)
            LookFreecam(view, key, deltatime);

        break;
    };
}

void Map::KeyPress(Viewport& view, ImGuiKey key)
{
    const float zoomfactor = sqrtf(2.0);
    const float minzoom = 8.0;
    const float maxzoom = max_map_size * 2;

    int i;
    std::unordered_set<int>::iterator it;

    Eigen::Vector3f basis[3], add;
    float gridsize;
    std::unordered_set<int> newselection;

    // so that we dont get messed up by tool hotkeys
    if(ImGui::GetIO().KeyShift)
        return;

    switch(key)
    {
    case ImGuiKey_LeftBracket:
        if(gridlevel)
            gridlevel--;
        
        break;
    case ImGuiKey_RightBracket:
        if(gridlevel < max_grid_level)
            gridlevel++;

        break;
    case ImGuiKey_Minus:
        if(view.type != Viewport::FREECAM && view.zoom < maxzoom)
            view.zoom *= zoomfactor;

        break;
    case ImGuiKey_Equal:
        if(view.type != Viewport::FREECAM && view.zoom > minzoom)
            view.zoom /= zoomfactor;

        break;
    case ImGuiKey_W:
    case ImGuiKey_A:
    case ImGuiKey_S:
    case ImGuiKey_D:
        if(view.type != Viewport::FREECAM)
            PanOrtho(view, key);

        break;
    case ImGuiKey_Escape:
        if(this->tool == TOOL_ENTITY)
            this->placingentity = false;

        if(this->tool == TOOL_BRUSH)
            nbrushcorners = 0;

        if(this->tool == TOOL_PLANE)
        {
            this->ntriplane = 0;
            this->drawingtriplane = false;
            this->triplaneselection.clear();
        }

        break;
    case ImGuiKey_Enter:
        if(nbrushcorners == 2)
            this->FinalizeBrush();
        if(tool == TOOL_VERTEX)
            this->FinalizeVertexEdit();
        if(tool == TOOL_PLANE)
            this->FinalizePlane();
        if(tool == TOOL_ENTITY)
            this->FinalizeEntity();

        break;
    case ImGuiKey_Backspace:
    case ImGuiKey_Delete:
        if(this->tool == TOOL_SELECT)
            this->DeleteSelected();

        break;
    case ImGuiKey_F:
        if(tool == TOOL_PLANE)
        {
            std::swap(this->triplane[0], this->triplane[this->ntriplane-1]);
            newselection.clear();
            for(it=this->triplaneselection.begin(); it!=this->triplaneselection.end(); it++)
            {
                if(*it > this->ntriplane)
                    newselection.insert(*it);
                else
                    newselection.insert(this->ntriplane - 1 - *it);
            }
            this->triplaneselection = newselection;
        }

        break;
    case ImGuiKey_UpArrow:
    case ImGuiKey_LeftArrow:
    case ImGuiKey_DownArrow:
    case ImGuiKey_RightArrow:
        if(tool == TOOL_VERTEX || tool == TOOL_PLANE || tool == TOOL_SELECT)
        {
            if(view.type == Viewport::FREECAM)
                break;
            
            view.GetViewBasis(basis);
            gridsize = (float) (1 << this->gridlevel);
            if(key == ImGuiKey::ImGuiKey_UpArrow)
                add = basis[2] * gridsize;
            if(key == ImGuiKey::ImGuiKey_LeftArrow)
                add = basis[1] * -gridsize;
            if(key == ImGuiKey::ImGuiKey_DownArrow)
                add = basis[2] * -gridsize;
            if(key == ImGuiKey::ImGuiKey_RightArrow)
                add = basis[1] * gridsize;
            
            if(tool == TOOL_VERTEX)
            {
                MoveVertexPoints(add);
            }
            else if(tool == TOOL_PLANE)
            {
                if(this->drawingtriplane)
                    return;
                
                for(it=this->triplaneselection.begin(); it!=this->triplaneselection.end(); it++)
                    this->triplane[*it] += add;
            }
            else if(tool == TOOL_SELECT)
            {
                if(this->selectiontype == SELECT_ENTITY)
                {
                    for(it=this->entselection.begin(); it!=this->entselection.end(); it++)
                        this->entities[*it].Move(add);
                }
                else
                {
                    for(i=0; i<this->entities.size(); i++)
                        this->entities[i].MoveSelected(*this, add);
                }
            }
        }

        break;
    default:
        break;
    }
}

void Map::Click(const Viewport& view, const Eigen::Vector2f& mousepos, ImGuiMouseButton_ button)
{
    const float pixelradius = 2.0;

    int i, j;

    Eigen::Vector3f basis[3], p, clickpos, o, r;
    Eigen::Vector2f camplane;
    int icorner;
    int realgridsize;
    float curdist, bestdist;
    int bestent;
    float viewheight, worldradius;

    switch(this->tool)
    {
    case TOOL_SELECT:
        if(view.type != Viewport::FREECAM)
            return;

        view.GetRay(mousepos, &o, &r);

        if(!ImGui::IsKeyDown(ImGuiKey_LeftShift))
                ClearSelection();

        bestent = -1;
        for(i=0; i<entities.size(); i++)
        {
            if(!entities[i].RayIntersects(o, r, &curdist))
                continue;

            if(bestent < 0 || curdist < bestdist)
            {
                bestdist = curdist;
                bestent = i;
            }
        }

        if(bestent < 0)
            return;

        entities[bestent].Select(o, r, bestent, *this);

        break;
    case TOOL_VERTEX:
        view.GetRay(mousepos, &o, &r);

        for(i=0; i<entities.size(); i++)
        {
            if(selectiontype == SELECT_ENTITY && !entselection.contains(i))
                continue;
            entities[i].SelectVertex(o, r, *this, view);
        }

        break;
    case TOOL_PLANE:
        if(!this->drawingtriplane && !this->ntriplane)
        {
            if(view.type == Viewport::FREECAM)
                break;

            view.GetViewBasis(basis);
            basis[2] *= view.zoom;
            basis[1] *= view.zoom * (float) view.canvassize[0] / (float) view.canvassize[1];
            realgridsize = 1 << this->gridlevel;
            p = view.pos + mousepos[0] * basis[1] + mousepos[1] * basis[2];
            p[view.type] = 0;
            for(i=0; i<3; i++)
                p[i] = (roundf(p[i] / realgridsize)) * realgridsize;
            this->triplane[0] = this->triplane[2] = p;
            r = Eigen::Vector3f::Zero();
            r[view.type] = 1 << this->gridlevel;
            this->triplane[1] = this->triplane[0] + r;
            this->ntriplane = 3;
            this->drawingtriplane = true;
        }
        else if(this->drawingtriplane)
        {
            if(view.type == Viewport::FREECAM)
                break;
            this->drawingtriplane = false;
        }
        else
        {
            view.GetRay(mousepos, &o, &r);
            if(!ImGui::IsKeyDown(ImGuiKey_LeftShift))
                this->triplaneselection.clear();

            view.GetViewBasis(basis);
            if(view.type == Viewport::FREECAM)
                viewheight = tanf(DEG2RAD(view.fov) / 2.0) * 2.0;
            else
                viewheight = view.zoom * 2.0;

            for(i=0; i<this->ntriplane; i++)
            {
                worldradius = pixelradius / (float) view.canvassize[1] * viewheight;

                if(view.type == Viewport::FREECAM)
                    worldradius *= (this->triplane[i] - view.pos).dot(basis[0]);

                if(!Mathlib::RayCuboid(o, r, this->triplane[i], worldradius).hit)
                    continue;

                if(!this->triplaneselection.contains(i))
                    this->triplaneselection.insert(i);
                else
                    this->triplaneselection.erase(i);
            }
        }

        break;
    case TOOL_BRUSH:
        if(view.type == Viewport::FREECAM)
            return;

        if(button != ImGuiMouseButton_Left && button != ImGuiMouseButton_Right)
            return;
        if(button == ImGuiMouseButton_Right && !nbrushcorners)
            return;
        if(button == ImGuiMouseButton_Left)
            icorner = 0;
        if(button == ImGuiMouseButton_Right)
            icorner = 1;

        view.GetRay(mousepos, &o, NULL);
        if(!nbrushcorners || (icorner == 1 && nbrushcorners == 1))
            o[view.type] = 0;
        else
            o[view.type] = brushcorners[icorner][view.type];

        if(!this->nbrushcorners || (this->nbrushcorners == 1 && icorner == 1))
            this->nbrushcorners++;

        realgridsize = 1 << gridlevel;
        for(i=0; i<3; i++)
            this->brushcorners[icorner][i] = ((int) roundf(o[i] / (float) realgridsize)) << gridlevel;
        
        break;
    case TOOL_ENTITY:
        if(view.type == Viewport::FREECAM)
            return;

        if(button != ImGuiMouseButton_Left)
            return;

        view.GetRay(mousepos, &o, NULL);
        if(!placingentity)
            o[view.type] = 0;
        else
            o[view.type] = workingentity[view.type];

        realgridsize = 1 << gridlevel;
        for(i=0; i<3; i++)
            workingentity[i] = ((int) roundf(o[i] / (float) realgridsize)) << gridlevel;
        
        placingentity = true;
        
        break;
    }
}

void Map::MouseUpdate(const Viewport& view, const Eigen::Vector2f& mousepos)
{
    int i;

    Eigen::Vector3f basis[3], p;
    int realgridsize;

    switch(this->tool)
    {
    case TOOL_PLANE:
        if(!this->drawingtriplane)
            break;
        
        if(view.type == Viewport::FREECAM)
            break;

        view.GetViewBasis(basis);
        basis[2] *= view.zoom;
        basis[1] *= view.zoom * (float) view.canvassize[0] / (float) view.canvassize[1];
        realgridsize = 1 << this->gridlevel;
        p = view.pos + mousepos[0] * basis[1] + mousepos[1] * basis[2];
        p[view.type] = 0;
        for(i=0; i<3; i++)
            p[i] = (roundf(p[i] / realgridsize)) * realgridsize;
        this->triplane[2] = p;

        break;
    }
}

void Map::Render(const Viewport& view)
{
    int i;

    const float axislen = 64;

    glPushMatrix();
    SetupFrame(view);

    glDisable(GL_DEPTH_TEST);
    DrawGrid(view);
    glEnable(GL_DEPTH_TEST);

    // since in wireframe selected things should appear over unselected things
    for(i=0; i<this->entities.size(); i++)
        this->entities[i].Draw(view, i, *this, false);

    glDisable(GL_DEPTH_TEST);

    glPointSize(4.0);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(axislen, 0, 0);
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, axislen, 0);
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, axislen);
    glEnd();

    glEnable(GL_DEPTH_TEST);

    for(i=0; i<this->entities.size(); i++)
        this->entities[i].Draw(view, i, *this, true);
    DrawWorkingEnt(view);
    DrawWorkingBrush(view);
    DrawTriplane(view);

    glColor3f(1, 1, 1);

    glPopMatrix();
}

void Map::NewMap(void)
{
    Entity *worldspawn;

    this->LoadConfig();

    this->texarchives.clear();
    this->texarchives.push_back("aegis.tpk");
    this->LoadTextures();

    this->path = "";

    this->entselection.clear();
    this->entities.resize(1);
    worldspawn = &this->entities.back();

    worldspawn->pairs["classname"] = "worldspawn";
    worldspawn->brushes.clear();

    this->SwitchTool(TOOL_SELECT);
    this->selectiontype = SELECT_BRUSH;
}

void Map::Save(void)
{
    int i;

    int e, b, p;
    Entity *ent;
    Brush *br;
    Plane *pl;

    Maplib::MapFile file;
    Maplib::entity_t *fent;
    Maplib::brush_t *fbr;
    Maplib::plane_t *fpl;
    
    file.ents.resize(this->entities.size());
    for(e=0, ent=this->entities.data(); e<this->entities.size(); e++, ent++)
    {
        fent = &file.ents[e];
        fent->keys = ent->pairs;

        fent->brushes.resize(ent->brushes.size());
        for(b=0, br=ent->brushes.data(); b<ent->brushes.size(); b++, br++)
        {
            fbr = &fent->brushes[b];

            for(p=0, pl=br->planes.data(); p<br->planes.size(); p++, pl++)
            {
                if(pl->poly.size() < 3)
                    continue;

                fbr->planes.push_back({});
                fpl = &fbr->planes.back();

                for(i=0; i<3; i++)
                    fpl->triplane[i] = pl->poly[i * (pl->poly.size() / 3)].cast<int>();

                for(i=0; i<2; i++)
                {
                    fpl->texbasis[i] = Eigen::Vector3f(0, 0, 0);
                    fpl->texoffs[i] = 0;
                }

                fpl->texname = "";
            }
        }
    }

    file.Write(this->path);
}

void Map::Load(const std::string& path)
{
    int i;

    int e, b, p;
    Maplib::entity_t *fent;
    Maplib::brush_t *fbr;
    Maplib::plane_t *fpl;

    Maplib::MapFile file;
    Entity *ent;
    Brush *br;
    Plane *pl;

    file = Maplib::MapFile::Load(path);
    if(!file.ents.size())
        return;

    this->NewMap();
    this->path = path;

    this->entselection.clear();
    this->entities.resize(file.ents.size());
    for(e=0, fent=file.ents.data(); e<file.ents.size(); e++, fent++)
    {
        ent = &this->entities[e];
        
        ent->pairs = fent->keys;
        ent->brushes.resize(fent->brushes.size());
        for(b=0, fbr=fent->brushes.data(); b<fent->brushes.size(); b++, fbr++)
        {
            br = &ent->brushes[b];
            
            br->planes.resize(fbr->planes.size());
            for(p=0, fpl=fbr->planes.data(); p<fbr->planes.size(); p++, fpl++)
            {
                pl = &br->planes[p];

                pl->normal = ((fpl->triplane[1] - fpl->triplane[0]).cross(fpl->triplane[2] - fpl->triplane[0])).cast<float>();
                pl->normal.normalize();
                pl->d = pl->normal.dot(fpl->triplane[0].cast<float>());
            }
            br->MakeFaces();
        }
    }
}

void Map::LoadFgd(void)
{
    this->fgd = Fgdlib::FgdFile::Load(this->cfg.pairs["fgd"]);
}

void Map::LoadTextures(void)
{
    int i;

    this->texmanager.ClearAll();
    for(i=0; i<this->texarchives.size(); i++)
        this->texmanager.LoadArchive(this->texarchives[i].c_str());
}