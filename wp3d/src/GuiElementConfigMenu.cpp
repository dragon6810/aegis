#include "GuiElementConfigMenu.h"

#include <ImGuiFileDialog.h>

GuiElementConfigMenu::GuiElementConfigMenu(Map& map) : GuiElement(map), map(map)
{
    this->showmenu = false;
}

void GuiElementConfigMenu::Show(void)
{
    this->showmenu = true;
    this->workingcfg = map.cfg;
}

void GuiElementConfigMenu::Draw(void)
{
    if(!this->showmenu)
        return;

    Cfglib::CfgFile oldmapcfg;
    std::string fgdlabel;
    IGFD::FileDialogConfig dialogconfig;

    if (ImGui::Begin("Options", &this->showmenu))
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
            this->showmenu = false;

        ImGui::End();

        if (ImGuiFileDialog::Instance()->Display("OpenFgdFileKey") && ImGuiFileDialog::Instance()->IsOk())
        {
            workingcfg.pairs["fgd"] = ImGuiFileDialog::Instance()->GetFilePathName();

            ImGuiFileDialog::Instance()->Close();
        }
    }
}