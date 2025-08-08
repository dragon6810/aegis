#include "GuiElementTextureMapping.h"

GuiElementTextureMapping::GuiElementTextureMapping(Map& map) : GuiElement(map), map(map)
{

}

std::vector<Plane*> GuiElementTextureMapping::GetSelectedFaces(void)
{
    int i, j, k;
    Entity *ent;
    Brush *br;
    Plane *pl;

    std::vector<Plane*> planes;

    planes.clear();
    for(i=0, ent=map.entities.data(); i<map.entities.size(); i++, ent++)
    {
        if(map.selectiontype == Map::SELECT_ENTITY && !map.entselection.contains(i))
            continue;

        for(j=0, br=ent->brushes.data(); j<ent->brushes.size(); j++, br++)
        {
            if(map.selectiontype == Map::SELECT_BRUSH && !ent->brselection.contains(j))
                continue;

            for(k=0, pl=br->planes.data(); k<br->planes.size(); k++, pl++)
            {
                if(map.selectiontype == Map::SELECT_PLANE && !br->plselection.contains(k))
                    continue;

                planes.push_back(pl);
            }
        }
    }

    return planes;
}

void GuiElementTextureMapping::AlignGrid(void)
{
    int i;

    std::vector<Plane*> planes;

    planes = this->GetSelectedFaces();
    for(i=0; i<planes.size(); i++)
        planes[i]->AlignTexGrid();
}

void GuiElementTextureMapping::AlignFace(void)
{
    int i;

    std::vector<Plane*> planes;

    planes = this->GetSelectedFaces();
    for(i=0; i<planes.size(); i++)
        planes[i]->AlignTexFace();
}

void GuiElementTextureMapping::Shift(Eigen::Vector2i shift)
{
    int i, j;

    std::vector<Plane*> planes;

    planes = this->GetSelectedFaces();
    for(i=0; i<planes.size(); i++)
        for(j=0; j<2; j++)
            planes[i]->texshift[j] += shift[j];
}

void GuiElementTextureMapping::Scale(Eigen::Vector2f scale)
{
    int i, j;

    std::vector<Plane*> planes;

    planes = this->GetSelectedFaces();
    for(i=0; i<planes.size(); i++)
        for(j=0; j<2; j++)
            planes[i]->texbasis[j] *= scale[j];
}

void GuiElementTextureMapping::Rotate(float degrees)
{
    int i, j;

    float radians;
    Eigen::Quaternionf q;
    Eigen::Matrix3f rot;

    std::vector<Plane*> planes;

    radians = DEG2RAD(degrees);

    planes = this->GetSelectedFaces();
    for(i=0; i<planes.size(); i++)
    {
        q = Eigen::Quaternionf(Eigen::AngleAxisf(radians, planes[i]->normal));
        rot = q.toRotationMatrix();
        for(j=0; j<2; j++)
            planes[i]->texbasis[j] = rot * planes[i]->texbasis[j];
    }
}

void GuiElementTextureMapping::DrawQuickActions(void)
{
    int shiftamount;
    float width;
    ImVec2 btnsize;
    float degrees;

    width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0;
    btnsize = { width, 0 };

    if(ImGui::Button("Align to Grid", btnsize))
        this->AlignGrid();
    ImGui::SameLine();
    if(ImGui::Button("Align to Face", btnsize))
        this->AlignFace();

    shiftamount = 2;
    if(ImGui::IsKeyDown(ImGuiKey_ModCtrl))
        shiftamount >>= 1;
    if(ImGui::IsKeyDown(ImGuiKey_ModShift))
        shiftamount <<= 1;

    if(ImGui::Button("Shift Left", btnsize))
        this->Shift(Eigen::Vector2i(shiftamount, 0));
    ImGui::SameLine();
    if(ImGui::Button("Shift Right", btnsize))
        this->Shift(Eigen::Vector2i(-shiftamount, 0));
    if(ImGui::Button("Shift Down", btnsize))
        this->Shift(Eigen::Vector2i(0, shiftamount));
    ImGui::SameLine();
    if(ImGui::Button("Shift Up", btnsize))
        this->Shift(Eigen::Vector2i(0, -shiftamount));

    if(ImGui::Button("Shrink X", btnsize))
        this->Scale(Eigen::Vector2f(2.0, 1));
    ImGui::SameLine();
    if(ImGui::Button("Stretch X", btnsize))
        this->Scale(Eigen::Vector2f(0.5, 1));
    if(ImGui::Button("Shrink Y", btnsize))
        this->Scale(Eigen::Vector2f(1, 2.0));
    ImGui::SameLine();
    if(ImGui::Button("Stretch Y", btnsize))
        this->Scale(Eigen::Vector2f(1, 0.5));

    degrees = 15;
    if(ImGui::IsKeyDown(ImGuiKey_ModCtrl))
        degrees /= 3.0;
    if(ImGui::IsKeyDown(ImGuiKey_ModShift))
        degrees *= 3.0;

    if(ImGui::Button("Rotate CCW", btnsize))
        this->Rotate(degrees);
    ImGui::SameLine();
    if(ImGui::Button("Rotate CW", btnsize))
        this->Rotate(-degrees);
}

void GuiElementTextureMapping::Draw(void)
{
    ImGui::Begin("Texture Mapping");

    this->DrawQuickActions();

    ImGui::End();
}