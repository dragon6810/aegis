#include "Gui.h"

#include <chrono>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>
#include <imgui_stdlib.h>
#include <ImGuiFileDialog.h>

#include "GuiElementMenuBar.h"
#include "GuiElementToolBar.h"
#include "GuiElementToolSettings.h"
#include "GuiElementViewport.h"
#include "Map.h"

void Gui::Setup(GLFWwindow* win)
{
    int i;

    ImFont* opensans;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.Fonts->AddFontFromFileTTF("resource/OpenSans-Regular.ttf");

    ImGui::StyleColorsDark();
    ApplyStyle();
    
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL2_Init();

    map.NewMap();

    this->elements.push_back(std::make_unique<GuiElementMenuBar>(GuiElementMenuBar(this->map)));
    this->elements.push_back(std::make_unique<GuiElementToolBar>(GuiElementToolBar(this->map)));
    this->elements.push_back(std::make_unique<GuiElementToolSettings>(GuiElementToolSettings(this->map)));
    for(i=0; i<Viewport::NTYPES; i++)
        this->elements.push_back(std::make_unique<GuiElementViewport>(GuiElementViewport(this->map, (Viewport::viewporttype_e) i)));
}

void Gui::ApplyStyle(void)
{
    const ImVec4 windowbg = ImVec4(0.15, 0.15, 0.15, 1.0);
    const ImVec4 tabbar = ImVec4(0.05, 0.05, 0.05, 1.0);
    const ImVec4 button = ImVec4(0.2, 0.2, 0.2, 1.0);
    const ImVec4 active = ImVec4(0.25, 0.25, 0.25, 1.0);
    const ImVec4 hover = ImVec4(0.3, 0.3, 0.3, 1.0);
    const ImVec4 border = ImVec4(0.5, 0.5, 0.5, 1.0);

    ImGuiStyle *style;

    style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_WindowBg] = windowbg;
    style->Colors[ImGuiCol_TitleBg] = tabbar;
    style->Colors[ImGuiCol_TitleBgActive] = tabbar;
    style->Colors[ImGuiCol_Tab] = tabbar;
    style->Colors[ImGuiCol_TabHovered] = hover;
    style->Colors[ImGuiCol_TabActive] = windowbg;
    style->Colors[ImGuiCol_TabUnfocused] = tabbar;
    style->Colors[ImGuiCol_TabUnfocusedActive] = tabbar;
    style->Colors[ImGuiCol_Border] = border;
    style->Colors[ImGuiCol_TabSelectedOverline] = border;
    style->Colors[ImGuiCol_Button] = button;
    style->Colors[ImGuiCol_ButtonActive] = active;
    style->Colors[ImGuiCol_ButtonHovered] = hover;
    style->Colors[ImGuiCol_FrameBg] = button;
    style->Colors[ImGuiCol_FrameBgActive] = active;
    style->Colors[ImGuiCol_FrameBgHovered] = hover;
    style->Colors[ImGuiCol_Separator] = hover;
    style->Colors[ImGuiCol_SeparatorHovered] = hover;
    style->Colors[ImGuiCol_SeparatorActive] = hover;
    style->Colors[ImGuiCol_ResizeGrip] = active;
    style->FrameRounding = 2;
    style->FrameBorderSize = 1;
    style->WindowMenuButtonPosition = ImGuiDir_None;
    style->TabBarOverlineSize = 0;
    style->TabBorderSize = 1;
}

void Gui::DrawEntityPairs(void)
{
    int i, j;
    std::map<std::string, std::string>::iterator it;

    static std::string newkey = "";
    static int selectedrow = -1;

    Entity *ent;
    int idef;
    Fgdlib::EntityDef *def;
    std::map<std::string, std::string> newpairs;
    std::string displayname;

    ent = &this->map.entities[0];
    if(this->map.selectiontype == Map::SELECT_ENTITY && this->map.entselection.size())
        ent = &this->map.entities[*this->map.entselection.begin()];

    ImGui::Begin("Entity Pairs", NULL, ImGuiWindowFlags_NoCollapse);

    ImGui::Checkbox("Smart Edit", &this->smarteedit);

    if(smarteedit)
    {
        if(ent->pairs.find("classname") == ent->pairs.end())
        {
            ImGui::End();
            return;
        }
        
        for(i=0, def=map.fgd.ents.data(); i<map.fgd.ents.size(); i++, def++)
            if(def->classname == ent->pairs["classname"])
                break;
        if(i >= map.fgd.ents.size())
        {
            ImGui::End();
            return;
        }
        idef = i;

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
                    newpairs.clear();
                    for(it=ent->pairs.begin(); it!=ent->pairs.end(); it++)
                    {
                        if(it->first == "classname")
                            continue;

                        for(j=0; j<def->pairs.size(); j++)
                            if(def->pairs[j].keyname == it->first)
                                break;
                        
                        if(j >= def->pairs.size())
                            continue;
                        
                        newpairs[it->first] = it->second;
                    }

                    newpairs["classname"] = def->classname;
                    ent->pairs = newpairs;

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

        ImGui::Separator();

        if(ImGui::BeginTable("smartpairs", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            def = &map.fgd.ents[idef];
            for(i=0; i<def->pairs.size(); i++)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if(def->pairs[i].displayname.size())
                    displayname = def->pairs[i].displayname;
                else
                    displayname = def->pairs[i].keyname;

                if(ImGui::Selectable(displayname.c_str(), this->selectedpair == i))
                    this->selectedpair = i;
                
                ImGui::TableSetColumnIndex(1);
                ImGui::PushItemWidth(-FLT_MIN);
                switch(def->pairs[i].type)
                {
                case Fgdlib::EntityPair::VALTYPE_STRING:
                    if(ent->pairs.find(def->pairs[i].keyname) == ent->pairs.end())
                        ent->pairs[def->pairs[i].keyname] = def->pairs[i].defstring;

                    ImGui::InputText(("##" + displayname).c_str(), &ent->pairs[def->pairs[i].keyname]);
                    break;
                case Fgdlib::EntityPair::VALTYPE_INTEGER:
                    if(ent->pairs.find(def->pairs[i].keyname) == ent->pairs.end())
                        ent->pairs[def->pairs[i].keyname] = std::to_string(def->pairs[i].defint);

                    ImGui::InputText(("##" + displayname).c_str(), &ent->pairs[def->pairs[i].keyname]);
                    break;
                default:
                    break;
                }
                ImGui::PopItemWidth();
            }

            ImGui::EndTable();
        }
    }
    else
    {
        this->selectedpair = -1;

        if (ImGui::BeginTable("kvpairs", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            for(it=ent->pairs.begin(), i=0; it!=ent->pairs.end(); it++, i++)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if(ImGui::Selectable(it->first.c_str(), selectedrow == i))
                {
                    if(selectedrow == i)
                        selectedrow = -1;
                    else
                        selectedrow = i;
                }
                
                ImGui::TableSetColumnIndex(1);
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::InputText(("##val_" + it->first).c_str(), &it->second);
                ImGui::PopItemWidth();
            }

            ImGui::EndTable();
        }

        if(ImGui::Button("Add Pair"))
            ImGui::OpenPopup("Add Entity Pair");

        ImGui::SameLine();
        if (selectedrow >= 0)
        {
            ImGui::SameLine();
            if(ImGui::Button("Delete Pair"))
            {
                it = ent->pairs.begin();
                std::advance(it, selectedrow);
                ent->pairs.erase(it);
                selectedrow = -1;
            }
        }
    
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

    ImGui::End();
}

void Gui::DrawPairHelper(void)
{
    int i;

    Entity *ent;
    Fgdlib::EntityDef *def;

    ImGui::Begin("Entity Pair Helper", NULL, ImGuiWindowFlags_NoCollapse);
    
    if(!this->smarteedit)
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
    ImGui::TextWrapped(map.fgd.ents[i].pairs[this->selectedpair].description.c_str());

    ImGui::End();
}

void Gui::Draw()
{
    int i;

    float deltatime;
    uint64_t curframe, msdelta;
    ImGuiDockNodeFlags dockspaceflags;

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

    DrawEntityPairs();
    DrawPairHelper();

    curframe = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    if(!lastframe)
        lastframe = curframe;
    msdelta = curframe - lastframe;
    deltatime = (float) msdelta / 1000.0;

    for(i=0; i<this->elements.size(); i++)
        this->elements[i]->Draw();

    lastframe = curframe;
}

void Gui::FinishFrame()
{
    GLFWwindow *ctx;

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    
    ctx = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(ctx);
}
