#include "GuiWindow.h"

#include "Game.h"

#include "GUI.h"

GuiWindow::GuiWindow()
{
	
}


GuiWindow::~GuiWindow()
{
	Deregister();
}

void GuiWindow::Create()
{
	Register();
}

bool GuiWindow::IsActive()
{
	return isactive;
}

void GuiWindow::Hide()
{
	isactive = false;
}

void GuiWindow::Show()
{
	isactive = true;
}

void GuiWindow::Register()
{
	Game::GetGame().gui.RegisterWindow(this);
}

void GuiWindow::Deregister()
{
	Game::GetGame().gui.DeregisterWindow(this);
}