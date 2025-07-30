#include "Map.h"

#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
        pl->normal = Eigen::Vector3f(0, 0, 0);
        pl->normal[i] = -1;
        pl->d = -bb[0][i];

        pl = &br->planes[i * 2 + 1];
        pl->normal = Eigen::Vector3f(0, 0, 0);
        pl->normal[i] = 1;
        pl->d = bb[1][i];
    }

    br->MakeFaces();

    nbrushcorners = 0;
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

void Map::SetupPlanePoints(bool allplanes)
{
    int i, j, k;
    Entity *ent;
    Brush *br;

    for(i=0; i<this->entities.size(); i++)
    {
        if(!allplanes && this->selectiontype == SELECT_ENTITY && !this->entselection.contains(i))
            continue;

        ent = &this->entities[i];
        for(j=0; j<ent->brushes.size(); j++)
        {
            if(!allplanes && this->selectiontype == SELECT_BRUSH && !ent->brselection.contains(j))
                continue;

            br = &ent->brushes[j];
            for(k=0; k<br->planes.size(); k++)
            {
                if(!allplanes && this->selectiontype == SELECT_PLANE && !br->plselection.contains(k))
                    continue;
                
                br->planes[k].UpdateTriplane();
            }
        }
    }
}

void Map::MovePlanePoints(Eigen::Vector3f add)
{
    int i, j, k, l;

    Entity *ent;
    Brush *br;
    Plane *pl;

    bool rebuildbr, rebuildpl;

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
            rebuildbr = false;
            for(k=0; k<br->planes.size(); k++)
            {
                if(this->selectiontype == SELECT_PLANE && !br->plselection.contains(k))
                    continue;

                pl = &br->planes[k];
                rebuildpl = false;
                for(l=0; l<3; l++)
                {
                    if(!pl->triplaneselection.contains(l))
                        continue;

                    rebuildpl = true;
                    pl->triplane[l] += add;
                }

                if(rebuildpl)
                {
                    rebuildbr = true;
                    pl->UpdateStandard();
                }
            }

            if(rebuildbr)
                br->MakeFaces();
        }
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

    glBegin(GL_LINES);
    for(i=firstlevel; i<=max_grid_level; i++)
    {
        linedistance = 1 << i;
        brightness = 1.0 / (float) colcycle * (float) ((i % (colcycle)) + 1);
        brightness *= (maxbrightness - minbrightness);
        brightness += minbrightness;
        glColor3f(brightness, brightness, brightness);
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

void Map::SwitchTool(tooltype_e type)
{
    if(type == this->tool)
        return;

    if(type == TOOL_PLANE)
        SetupPlanePoints(false);

    this->nbrushcorners = 0;
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

    Eigen::Vector3f basis[3], add;
    float gridsize;

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
        if(this->tool = TOOL_BRUSH)
            nbrushcorners = 0;

        break;
    case ImGuiKey_Enter:
        if(nbrushcorners == 2)
            this->FinalizeBrush();
        if(tool == TOOL_VERTEX)
            this->FinalizeVertexEdit();

        break;
    case ImGuiKey_UpArrow:
    case ImGuiKey_LeftArrow:
    case ImGuiKey_DownArrow:
    case ImGuiKey_RightArrow:
        if(tool == TOOL_PLANE || tool == TOOL_VERTEX)
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

            if(tool == TOOL_PLANE)
                MovePlanePoints(add);
            else if(tool == TOOL_VERTEX)
                MoveVertexPoints(add);
        }

        break;
    default:
        break;
    }
}

void Map::Click(const Viewport& view, const Eigen::Vector2f& mousepos, ImGuiMouseButton_ button)
{
    int i, j;

    Eigen::Vector3f basis[3], clickpos, o, r;
    Eigen::Vector2f camplane;
    int icorner;
    int realgridsize;
    float curdist, bestdist;
    int bestent;

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
        this->entities[i].Draw(view, i, *this);

    DrawWorkingBrush(view);

    glColor3f(1, 1, 1);

    glPopMatrix();
}

void Map::NewMap(void)
{
    Entity *worldspawn;

    this->entselection.clear();

    this->entities.resize(1);
    worldspawn = &this->entities.back();

    worldspawn->pairs["classname"] = "worldspawn";
    worldspawn->pairs["wp3dversion"] = "wp3d_v1";
    worldspawn->brushes.clear();

    this->SwitchTool(TOOL_SELECT);
    this->selectiontype = SELECT_BRUSH;
}