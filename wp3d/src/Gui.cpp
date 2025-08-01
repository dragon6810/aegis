#include "Gui.h"

#include <chrono>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl2.h>
#include <imgui_stdlib.h>
#include <ImGuiFileDialog.h>

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

void Gui::ViewportInput(void)
{

}

void Gui::DrawMenuBar(void)
{
    IGFD::FileDialogConfig config;

    if (ImGui::BeginMainMenuBar()) 
    {
        if (ImGui::BeginMenu("File")) 
        {
            if (ImGui::MenuItem("New", "Ctrl+N")) 
            {
                map.NewMap();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                if(map.path == "")
                {
                    config = {};
                    config.path = ".";
		            ImGuiFileDialog::Instance()->OpenDialog("SaveMapFileKey", "Choose Map File", ".map", config);
                }
                else
                {
                    this->map.Save();
                }
            }
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                config = {};
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("OpenMapFileKey", "Choose Map File", ".map", config);
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) 
        {
            if (ImGui::MenuItem("Options")) 
            {
                this->showconfigwindow = true;
                workingcfg = map.cfg;
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGuiFileDialog::Instance()->Display("SaveMapFileKey") && ImGuiFileDialog::Instance()->IsOk())
    {
        this->map.path = ImGuiFileDialog::Instance()->GetFilePathName();
        this->map.Save();

        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("OpenMapFileKey") && ImGuiFileDialog::Instance()->IsOk())
    {
        this->map.Load(ImGuiFileDialog::Instance()->GetFilePathName());

        ImGuiFileDialog::Instance()->Close();
    }
}

void Gui::DrawConfigMenu(void)
{
    if(!this->showconfigwindow)
        return;

    Cfglib::CfgFile oldmapcfg;
    std::string fgdlabel;
    IGFD::FileDialogConfig dialogconfig;

    if (ImGui::Begin("Options", &this->showconfigwindow))
    {
        fgdlabel = "FGD File: ";
        if (!workingcfg.pairs["fgd"].empty())
            fgdlabel += std::filesystem::path(workingcfg.pairs["fgd"]).filename().string();
        else
            fgdlabel += "<none>";
        if(ImGui::Button(fgdlabel.c_str()))
        {
            dialogconfig = {};
            if(workingcfg.pairs["fgd"] == "")
                dialogconfig.path = ".";
            else
                dialogconfig.filePathName = workingcfg.pairs["fgd"];
            ImGuiFileDialog::Instance()->OpenDialog("OpenFgdFileKey", "Choose FGD File", ".fgd", dialogconfig);
        }

        ImGui::Separator();

        if(ImGui::Button("Apply"))
        {
            oldmapcfg = map.cfg;
            map.cfg = workingcfg;
            map.cfg.Write(map.cfgpath.c_str());
            if(map.cfg.pairs["fgd"] != oldmapcfg.pairs["fgd"])
                map.LoadFgd();
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel"))
            this->showconfigwindow = false;

        ImGui::End();

        if (ImGuiFileDialog::Instance()->Display("OpenFgdFileKey") && ImGuiFileDialog::Instance()->IsOk())
        {
            workingcfg.pairs["fgd"] = ImGuiFileDialog::Instance()->GetFilePathName();

            ImGuiFileDialog::Instance()->Close();
        }
    }

    if(!this->showconfigwindow)
        workingcfg = map.cfg;
}

void Gui::DrawToolBar(void)
{
    const auto tool = [&](const char* label, Map::tooltype_e toolId, const char* shortcut, const char* description)
    {
        if (map.tool == toolId)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        else
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_Button]);

        if (ImGui::Button(label, ImVec2(ImGui::GetContentRegionAvail().x, 0.0)))
            map.SwitchTool(toolId);

        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(shortcut);
            ImGui::TextUnformatted(description);
            ImGui::EndTooltip();
        }
    };

    bool shift;

    shift = ImGui::GetIO().KeyShift;
    if (shift && ImGui::IsKeyPressed(ImGuiKey_S)) map.SwitchTool(Map::TOOL_SELECT);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_W)) map.SwitchTool(Map::TOOL_TRANSLATE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_E)) map.SwitchTool(Map::TOOL_ROTATE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_R)) map.SwitchTool(Map::TOOL_SCALE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_Z)) map.SwitchTool(Map::TOOL_VERTEX);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_X)) map.SwitchTool(Map::TOOL_PLANE);
    if (shift && ImGui::IsKeyPressed(ImGuiKey_B)) map.SwitchTool(Map::TOOL_BRUSH);

    ImGui::Begin("Tool Bar", NULL, ImGuiWindowFlags_NoCollapse);

    tool("Select",         Map::TOOL_SELECT,    "Select Tool (Shift + S)", "Can be used to select vertices,\nplanes, brushes, or entities");
    tool("Translate",      Map::TOOL_TRANSLATE, "Translate Tool (Shift + W)", "Can be used to translate vertices,\nplanes, brushes, or entities");
    tool("Rotate",         Map::TOOL_ROTATE,    "Rotate Tool (Shift + E)", "Can be used to rotate brushes or\nentities");
    tool("Scale",          Map::TOOL_SCALE,     "Scale Tool (Shift + R)", "Can be used to scale brushes");
    tool("Vertex Editing", Map::TOOL_VERTEX,    "Vertex Editing Tool (Shift + Z)", "Can be used to modify vertex geometry");
    tool("Plane",          Map::TOOL_PLANE,     "Plane Tool (Shift + X)", "Can be used to create new planes");
    tool("Brush",          Map::TOOL_BRUSH,     "Brush Tool (Shift + B)", "Can be used to create new brushes");

    ImGui::End();
}

void Gui::DrawToolSettings(void)
{
    const char *selecttypenames[] = { "Select Planes", "Select Brushes", "Select Entities", };

    int i;

    int selectiontype;

    ImGui::Begin("Tool Settings", NULL, ImGuiWindowFlags_NoCollapse);

    switch(map.tool)
    {
    case Map::TOOL_SELECT:
        ImGui::Text("Selection Mode");

        for(i=0; i<(int) Map::SELECT_COUNT; i++)
            if(ImGui::IsKeyPressed((ImGuiKey) ((int) ImGuiKey_1 + i)))
                    map.selectiontype = (Map::selectiontype_e) i;

        selectiontype = (int) map.selectiontype;
        if (ImGui::BeginCombo("##SelectionModeDropdown", selecttypenames[selectiontype]))
        {
            for (i=0; i<(int) Map::SELECT_COUNT; i++)
            {
                if (ImGui::Selectable(selecttypenames[i], selectiontype == i))
                    selectiontype = i;
                if (selectiontype == i)
                    ImGui::SetItemDefaultFocus();

                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip((std::string(selecttypenames[i]) + " (" + std::to_string(i+1) + ")").c_str());
            }
            ImGui::EndCombo();
            map.selectiontype = (Map::selectiontype_e) selectiontype;
        }

        break;
    default:
        break;
    }

    ImGui::End();
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

void Gui::DrawRibbon(void)
{
    ImGui::Begin("Ribbon", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
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

    this->DrawMenuBar();
    this->DrawConfigMenu();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);

    DrawRibbon();
    DrawToolBar();
    DrawToolSettings();
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
