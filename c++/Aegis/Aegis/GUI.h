#pragma once

#include <GL/glew.h>

#include <string>

#include "defs.h"

#include "Waveform.h"
#include "TrueTypeFont.h"

#include "GuiWindow.h"

class GUI
{
public:
	void Reload();

	void RegisterWindow(GuiWindow* window);
	void DeregisterWindow(GuiWindow* window);

	void MouseDrag(int x, int y);
	void MouseDown(int x, int y);
	void MouseUp(int x, int y);

	void DrawScreen();
	void RenderWindow(int x, int y, int width, int height, std::string title);
private:
	color24_t windowcol;
	color24_t panelcol;
	color24_t windowclosecol;

	TrueTypeFont normal;
	TrueTypeFont bold;
	TrueTypeFont mono;
	TrueTypeFont icons;

	GLuint windowborder;
	GLuint panelborder;
	GLuint windowcloseicon;

	int windowborderw;
	int windowborderh;

	waveform_t windowclosesound;

	int txtsmall;
	int txtmed;
	int txtlarge;

	bool dragging;
	int draggingwin;
	int startx;
	int starty;
	int mouseoffsx;
	int mouseoffsy;
	std::vector<GuiWindow*> windows;
};

