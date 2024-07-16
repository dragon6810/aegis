#pragma once
class GuiWindow
{
public:
	bool IsActive();

	void Hide();
	void Show();

	int GetX();
	int GetY();
	int GetW();
	int GetH();

	void SetX(int x);
	void SetY(int y);
	void SetW(int w);
	void SetH(int h);
protected:
	bool isactive;

	int x;
	int y;
	int w;
	int h;
};

