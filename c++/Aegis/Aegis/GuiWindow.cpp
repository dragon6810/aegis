#include "GuiWindow.h"

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