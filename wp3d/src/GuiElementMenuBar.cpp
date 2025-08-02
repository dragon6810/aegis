#include "GuiElementMenuBar.h"

#include <ImGuiFileDialog.h>

#include "Gui.h"

GuiElementMenuBar::GuiElementMenuBar(Map& map) : GuiElement(map), cfgmenu(map), map(map)
{

}

void GuiElementMenuBar::DrawMenuFile(void)
{
    IGFD::FileDialogConfig config;

    if (ImGui::BeginMenu("File")) 
    {
        if (ImGui::MenuItem("New", GUI_CONTROL_STRING " N")) 
            this->New();

        ImGui::Separator();

        if (ImGui::MenuItem("Open", GUI_CONTROL_STRING " O"))
            this->Open();
        
        ImGui::Separator();

        if (ImGui::MenuItem("Save", GUI_CONTROL_STRING " S"))
            this->Save();

        if (ImGui::MenuItem("Save As", GUI_CONTROL_STRING " " GUI_ALT_STRING " S"))
            this->SaveAs();

        ImGui::EndMenu();
    }
}

void GuiElementMenuBar::DrawMenuEdit(void)
{
    if (ImGui::BeginMenu("Edit")) 
    {
        if (ImGui::MenuItem("Options", GUI_CONTROL_STRING " ,"))
            this->Options();

        ImGui::EndMenu();
    }
}

void GuiElementMenuBar::DrawFileDialogs(void)
{
    if (ImGuiFileDialog::Instance()->Display("SaveMapFileKey"))
    {
        if(ImGuiFileDialog::Instance()->IsOk())
        {
            this->map.path = ImGuiFileDialog::Instance()->GetFilePathName();
            this->map.Save();
        }

        ImGuiFileDialog::Instance()->Close();
    }

    if (ImGuiFileDialog::Instance()->Display("OpenMapFileKey"))
    {
        if(ImGuiFileDialog::Instance()->IsOk())
            this->map.Load(ImGuiFileDialog::Instance()->GetFilePathName());

        ImGuiFileDialog::Instance()->Close();
    }
}

void GuiElementMenuBar::New(void)
{
    this->map.NewMap();
}

void GuiElementMenuBar::Open(void)
{
    IGFD::FileDialogConfig config;
    
    config = {};
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("OpenMapFileKey", "Choose Map File", ".map", config);
}

void GuiElementMenuBar::Save(void)
{
    IGFD::FileDialogConfig config;

    if(map.path == "")
        this->SaveAs();
    else
        this->map.Save();
}

void GuiElementMenuBar::SaveAs(void)
{
    IGFD::FileDialogConfig config;
    
    config = {};
    config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("SaveMapFileKey", "Choose Map File", ".map", config);
}

void GuiElementMenuBar::Options(void)
{
    this->cfgmenu.Show();
}

void GuiElementMenuBar::Draw(void)
{
    if (ImGui::BeginMainMenuBar()) 
    {
        this->DrawMenuFile();
        this->DrawMenuEdit();
        
        ImGui::EndMainMenuBar();
    }

    if(ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyDown(ImGuiKey_N))
        this->New();
    if(ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyDown(ImGuiKey_O))
        this->Open();
    if(ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyDown(ImGuiKey_S))
    {
        if(ImGui::IsKeyDown(ImGuiKey_ModAlt))
            this->SaveAs();
        else
            this->Save();
    }
    if(ImGui::IsKeyDown(ImGuiKey_ModCtrl) && ImGui::IsKeyDown(ImGuiKey_Comma))
        this->Options();

    this->DrawFileDialogs();

    this->cfgmenu.Draw();
}