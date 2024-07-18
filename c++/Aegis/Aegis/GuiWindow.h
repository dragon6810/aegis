#pragma once

#include <string>

class GuiWindow
{
public:
	GuiWindow();
	~GuiWindow();

	void Create();

	bool IsActive();

	void Hide();
	void Show();

	std::string GetTitle();
	int GetX();
	int GetY();
	int GetW();
	int GetH();

	void SetTitle(std::string title);
	void SetX(int x);
	void SetY(int y);
	void SetW(int w);
	void SetH(int h);

	bool dragging;
	int startx;
	int starty;
	int mouseoffsx;
	int mouseoffsy;
protected:
	bool isactive;

	std::string title;
	int x;
	int y;
	int w;
	int h;
private:
	void Register();
	void Deregister();
};

