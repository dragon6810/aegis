#include "GuiElementEntityEditor.h"

#include <imgui_stdlib.h>

GuiElementEntityEditor::GuiElementEntityEditor(Map& map) : GuiElement(map), map(map)
{

}

void GuiElementEntityEditor::DrawPairs(void)
{
    Entity *ent;

    ImGui::Begin("Entity Pairs", NULL, ImGuiWindowFlags_NoCollapse);

    ent = &this->map.entities[0];
    if(this->map.selectiontype == Map::SELECT_ENTITY && this->map.entselection.size())
        ent = &this->map.entities[*this->map.entselection.begin()];

    if(ImGui::Checkbox("Smart Edit", &this->smartedit))
        this->selectedpair = -1;

    if(this->smartedit)
        this->DrawPairsSmartEdit(ent);
    else
        this->DrawPairsRaw(ent);

    ImGui::End();
}

void GuiElementEntityEditor::DrawHelper(void)
{
    int i;

    Entity *ent;
    Fgdlib::EntityDef *def;

    ImGui::Begin("Entity Pair Helper", NULL, ImGuiWindowFlags_NoCollapse);
    
    if(!this->smartedit)
    {
        ImGui::End();
        return;
    }

    ent = &this->map.entities[0];
    if(this->map.selectiontype == Map::SELECT_ENTITY && this->map.entselection.size())
        ent = &this->map.entities[*this->map.entselection.begin()];

    if(ent->pairs.find("classname") == ent->pairs.end())
    {
        ImGui::End();
        return;
    }

    for(i=0; i<map.fgd.ents.size(); i++)
        if(map.fgd.ents[i].classname == ent->pairs["classname"])
            break;

    if(i >= map.fgd.ents.size())
    {
        ImGui::End();
        return;
    }

    def = &map.fgd.ents[i];

    ImGui::TextUnformatted(def->classname.c_str());
    if(def->description.size())
    {
        ImGui::Separator();
        ImGui::TextUnformatted(def->description.c_str());
    }

    if(this->selectedpair < 0)
    {
        ImGui::End();
        return;
    }

    ImGui::Separator();
    ImGui::TextWrapped("%s", map.fgd.ents[i].pairs[this->selectedpair].description.c_str());

    ImGui::End();
}

void GuiElementEntityEditor::DrawPairsSmartEdit(Entity* ent)
{
    int i;

    int idef;
    Fgdlib::EntityDef *def;

    if(!ent || ent->pairs.find("classname") == ent->pairs.end())
        return;
    
    for(i=0, def=map.fgd.ents.data(); i<map.fgd.ents.size(); i++, def++)
        if(def->classname == ent->pairs["classname"])
            break;

    if(i >= map.fgd.ents.size())
        return;

    idef = i;

    this->DrawClassDropdown(ent, idef);

    ImGui::Separator();

    if(ImGui::BeginTable("smartpairs", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        def = &map.fgd.ents[idef];
        for(i=0; i<def->pairs.size(); i++)
        {
            ImGui::TableNextRow();

            this->DrawSmartPair(ent, i, &def->pairs[i]);
        }

        ImGui::EndTable();
    }
}

void GuiElementEntityEditor::DrawPairsRaw(Entity* ent)
{
    int i;
    std::map<std::string, std::string>::iterator it;

    if (ImGui::BeginTable("kvpairs", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
    {
        for(it=ent->pairs.begin(), i=0; it!=ent->pairs.end(); it++, i++)
        {
            if(it->first == "classname" && it->second == "worldspawn")
                ImGui::BeginDisabled();

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            if(ImGui::Selectable(it->first.c_str(), this->selectedpair == i))
            {
                if(this->selectedpair == i)
                    this->selectedpair = -1;
                else
                    this->selectedpair = i;
            }
            
            ImGui::TableSetColumnIndex(1);
            ImGui::PushItemWidth(-FLT_MIN);
            ImGui::InputText(("##val_" + it->first).c_str(), &it->second);
            ImGui::PopItemWidth();

            if(it->first == "classname" && it->second == "worldspawn")
                ImGui::EndDisabled();
        }

        ImGui::EndTable();
    }

    if(ImGui::Button("Add Pair"))
        ImGui::OpenPopup("Add Entity Pair");

    ImGui::SameLine();
    if (selectedpair >= 0)
    {
        ImGui::SameLine();
        if(ImGui::Button("Delete Pair"))
        {
            it = ent->pairs.begin();
            std::advance(it, selectedpair);
            ent->pairs.erase(it);
            selectedpair = -1;
        }
    }

    this->DrawPairPopup(ent);
}

void GuiElementEntityEditor::DrawClassDropdown(Entity* ent, int idef)
{
    int i, j;
    std::map<std::string, std::string>::iterator it;

    Fgdlib::EntityDef *def;
    std::map<std::string, std::string> newpairs;

    if(ent->pairs["classname"] == "worldspawn")
        ImGui::BeginDisabled();

    ImGui::PushItemWidth(-FLT_MIN);
    
    if(ImGui::BeginCombo("##classdropdown", map.fgd.ents[idef].classname.c_str()))
    {
        for(i=0; i<map.fgd.ents.size(); i++)
        {
            if(map.fgd.ents[i].classname == "worldspawn")
                ImGui::BeginDisabled();

            if(ImGui::Selectable(map.fgd.ents[i].classname.c_str(), i == idef))
            {
                idef = i;
                def = &map.fgd.ents[idef];

                this->SwitchClass(ent, def);

                this->selectedpair = -1;
            }

            if(map.fgd.ents[i].classname == "worldspawn")
                ImGui::EndDisabled();
        }

        ImGui::EndCombo();
    }

    ImGui::PopItemWidth();
    
    if(ent->pairs["classname"] == "worldspawn")
        ImGui::EndDisabled();
}

void GuiElementEntityEditor::SwitchClass(Entity* ent, Fgdlib::EntityDef *newclass)
{
    int i;
    std::map<std::string, std::string>::iterator it;

    std::map<std::string, std::string> newpairs;

    newpairs.clear();
    for(it=ent->pairs.begin(); it!=ent->pairs.end(); it++)
    {
        if(it->first == "classname")
            continue;

        for(i=0; i<newclass->pairs.size(); i++)
            if(newclass->pairs[i].keyname == it->first)
                break;
        
        if(i >= newclass->pairs.size())
            continue;
        
        newpairs[it->first] = it->second;
    }

    newpairs["classname"] = newclass->classname;
    ent->pairs = newpairs;
}

void GuiElementEntityEditor::DrawSmartPair(Entity* ent, int ipair, Fgdlib::EntityPair* pair)
{
    std::string displayname;

    ImGui::TableSetColumnIndex(0);
    if(pair->displayname.size())
        displayname = pair->displayname;
    else
        displayname = pair->keyname;

    if(ImGui::Selectable(displayname.c_str(), this->selectedpair == ipair))
        this->selectedpair = ipair;

    switch(pair->type)
    {
    case Fgdlib::EntityPair::VALTYPE_STRING:
        if(ent->pairs.find(pair->keyname) == ent->pairs.end())
            ent->pairs[pair->keyname] = pair->defstring;
        break;
    case Fgdlib::EntityPair::VALTYPE_INTEGER:
        if(ent->pairs.find(pair->keyname) == ent->pairs.end())
            ent->pairs[pair->keyname] = std::to_string(pair->defint);
        break;
    default:
        break;
    }
    
    ImGui::TableSetColumnIndex(1);
    
    ImGui::PushItemWidth(-FLT_MIN);

    ImGui::InputText(("##" + displayname).c_str(), &ent->pairs[pair->keyname]);

    ImGui::PopItemWidth();
}

void GuiElementEntityEditor::DrawPairPopup(Entity* ent)
{
    std::string newkey;

    if (ImGui::BeginPopupModal("Add Entity Pair", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("New Key", &newkey);

        if (ImGui::Button("Confirm"))
        {
            if(newkey.size())
                ent->pairs[newkey] = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

void GuiElementEntityEditor::Draw(void)
{
    this->DrawPairs();
    this->DrawHelper();
}