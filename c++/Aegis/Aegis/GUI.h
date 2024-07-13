#pragma once

#include <GL/glew.h>

#include <string>

#include "defs.h"

#include "Waveform.h"
#include "TrueTypeFont.h"

class GUI
{
public:
	void Reload();

	void RenderWindow(int x, int y, int width, int height);
private:
	color24_t windowcol;
	color24_t panelcol;
	color24_t windowclosecol;

	TrueTypeFont icons;

	GLuint windowborder;
	GLuint panelborder;
	GLuint windowcloseicon;

	int windowborderw;
	int windowborderh;

	waveform_t windowclosesound;
};

