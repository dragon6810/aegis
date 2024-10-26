#pragma once

#include <string>

#include "TextPanel.h"

class GuiWindow
{
public:
	GuiWindow();
	~GuiWindow();

	void Create();

	bool IsActive();

	void Hide();
	void Show();

	std::string title;
	int x;
	int y;
	int w;
	int h;

	//std::vector<TextPanel> textpanels;
protected:
	bool isactive;
private:
	void Register();
	void Deregister();
};

