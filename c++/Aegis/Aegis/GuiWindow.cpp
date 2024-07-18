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

std::string GuiWindow::GetTitle()
{
	return title;
}

int GuiWindow::GetX()
{
	return x;
}

int GuiWindow::GetY()
{
	return y;
}

int GuiWindow::GetW()
{
	return w;
}

int GuiWindow::GetH()
{
	return h;
}

void GuiWindow::SetTitle(std::string title)
{
	this->title = title;
}

void GuiWindow::SetX(int x)
{
	this->x = x;
}

void GuiWindow::SetY(int y)
{
	this->y = y;
}

void GuiWindow::SetW(int w)
{
	this->w = w;
}

void GuiWindow::SetH(int h)
{
	this->h = h;
}

void GuiWindow::Register()
{
	Game::GetGame().gui.RegisterWindow(this);
}

void GuiWindow::Deregister()
{
	Game::GetGame().gui.DeregisterWindow(this);
}