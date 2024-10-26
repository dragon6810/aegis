#include "GUI.h"

#include "Print.h"
#include "Game.h"

#include "Targa.h"

void GUI::Reload()
{
	FILE* ptr;
	char line[256];
	char key[128];
	char val[128];

	ptr = fopen(std::string(Game::GetGame().gamedir + "/gui.txt").c_str(), "r");

	if (ptr == NULL)
	{
		Print::Aegis_Warning("Can't locate GUI resource file \"%s\".\n", std::string(Game::GetGame().gamedir + "/" + "gui.txt").c_str());
		return;
	}

	while (fgets(line, sizeof(line), ptr) != NULL)
	{
		// Skip blank lines
		if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0')
			continue;
		
		if (sscanf(line, "\"%127[^\"]\": ", key) != 1)
			continue;

		if (!strcmp(key, "Window Color"))
		{
			sscanf(line, "\"Window Color\": \"%d %d %d\"\n", (int*)&windowcol.r, (int*)&windowcol.g, (int*)&windowcol.b);
		}
		else if (!strcmp(key, "Panel Color"))
		{
			sscanf(line, "\"Panel Color\": \"%d %d %d\"\n", (int*)&panelcol.r, (int*)&panelcol.g, (int*)&panelcol.b);
		}
		else if (!strcmp(key, "Window Close Color"))
		{
			sscanf(line, "\"Window Close Color\": \"%d %d %d\"\n", (int*)&windowclosecol.r, (int*)&windowclosecol.g, (int*)&windowclosecol.b);
		}
		else if (!strcmp(key, "Regular"))
		{
			//if (sscanf(line, "\"Regular\": \"%127[^\"]\"\n", val))
			//	normal.Load(Game::GetGame().gamedir + "/" + val);
		}
		else if (!strcmp(key, "Bold"))
		{
			//if (sscanf(line, "\"Bold\": \"%127[^\"]\"\n", val))
			//	bold.Load(Game::GetGame().gamedir + "/" + val);
		}
		else if (!strcmp(key, "Mono"))
		{
			//if (sscanf(line, "\"Mono\": \"%127[^\"]\"\n", val))
			//	mono.Load(Game::GetGame().gamedir + "/" + val);
		}
		else if (!strcmp(key, "Icons"))
		{
			//if(sscanf(line, "\"Icons\": \"%127[^\"]\"\n", val))
			//	icons.Load(Game::GetGame().gamedir + "/" + val);
		}
		else if (!strcmp(key, "Window Border"))
		{
			if (sscanf(line, "\"Window Border\": \"%127[^\"]\"\n", val))
				windowborder = Targa::LoadTargaImage(Game::GetGame().gamedir + "/" + val, &windowborderw, &windowborderh);

			windowborderw >>= 1;
			windowborderh >>= 1;
		}
		else if (!strcmp(key, "Panel Border"))
		{
			if (sscanf(line, "\"Panel Border\": \"%127[^\"]\"\n", val))
				panelborder = Targa::LoadTargaImage(Game::GetGame().gamedir + "/" + val, nullptr, nullptr);
		}
		else if (!strcmp(key, "Window Close Sound"))
		{
			if (sscanf(line, "\"Window Close Sound\": \"%127[^\"]\"\n", val))
				windowclosesound = Waveform::LoadSound(Game::GetGame().gamedir + "/" + val);
		}
		else if (!strcmp(key, "Small Size"))
		{
			sscanf(line, "\"Small Size\": \"%d\"\n", &txtsmall);
		}
		else if (!strcmp(key, "Medium Size"))
		{
			sscanf(line, "\"Medium Size\": \"%d\"\n", &txtmed);
		}
		else if (!strcmp(key, "Large Size"))
		{
			sscanf(line, "\"Large Size\": \"%d\"\n", &txtlarge);
		}
		else
		{
			continue;
		}
	}

	fclose(ptr);
}

void GUI::RegisterWindow(GuiWindow* window)
{
	if (std::find(windows.begin(), windows.end(), window) == windows.end())
		windows.push_back(window);
}

void GUI::DeregisterWindow(GuiWindow* window)
{
	std::vector<GuiWindow*>::iterator it = std::find(windows.begin(), windows.end(), window);

	if (it != windows.end())
		windows.erase(it);
}

void GUI::MouseDrag(int x, int y)
{
	int i;

	GuiWindow* window;

	mousex = x;
	mousey = y;

	for (i = 0; i < windows.size(); i++)
	{
		if (!OverX(i, x, y) || !windows[i]->IsActive())
			continue;

		Game::GetGame().cursorshape = GLFW_HAND_CURSOR;
	}

	if (!dragging)
		return;

	window = windows[draggingwin];

	windows[draggingwin]->x = x - mouseoffsx;
	windows[draggingwin]->y = y - mouseoffsy;
}

void GUI::MouseDown(int x, int y)
{
	int i;

	GuiWindow* window;

	for (i = 0; i < windows.size(); i++)
	{
		window = windows[i];

		if (!window->IsActive())
			continue;

		if (x < window->x)
			continue;

		if (x > window->x + window->w)
			continue;

		if (y > window->y)
			continue;

		if (y < window->y - (windowborderh << 1))
			continue;

		if (OverX(i, x, y))
		{
			downx = true;
			draggingwin = i;
			return;
		}

		dragging = true;
		draggingwin = i;
		startx = window->x;
		starty = window->y;
		mouseoffsx = x - window->x;
		mouseoffsy = y - window->y;
	}
}

void GUI::MouseUp(int x, int y)
{
	GuiWindow* window;

	dragging = false;

	if (downx)
	{
		window = windows[draggingwin];

		if (OverX(draggingwin, x, y))
			window->Hide();

		downx = false;
	}
}

void GUI::DrawScreen()
{
	int i;

	for (i = windows.size() - 1; i >= 0; i--)
		RenderWindow(i);
}

void GUI::RenderWindow(int w)
{
	int xsize;

	GuiWindow* window;

	int x, y, width, height;
	std::string title;

	window = windows[w];
	x = window->x;
	y = window->y;
	width = window->w;
	height = window->h;
	title = window->title;

	xsize = (windowborderh << 1) - 6;

	if (!window->IsActive())
		return;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, windowborder);
	glEnable(GL_TEXTURE_2D);

	if (width < windowborderw << 1)
		width = windowborderw << 1;

	if (height < windowborderh << 1)
		height = windowborderh << 1;

	glBegin(GL_QUADS);

	// Top left corner
	glTexCoord2f(0.0, 0.0); glVertex2f(x, y - (windowborderh << 1));
	glTexCoord2f(0.0, 1.0); glVertex2f(x, y);
	glTexCoord2f(0.5, 1.0); glVertex2f(x + windowborderw, y);
	glTexCoord2f(0.5, 0.0); glVertex2f(x + windowborderw, y - (windowborderh << 1));

	// Top right corner
	glTexCoord2f(0.5, 0.0); glVertex2f(x + width - windowborderw, y - (windowborderh << 1));
	glTexCoord2f(0.5, 1.0); glVertex2f(x + width - windowborderw, y);
	glTexCoord2f(1.0, 1.0); glVertex2f(x + width, y);
	glTexCoord2f(1.0, 0.0); glVertex2f(x + width, y - (windowborderh << 1));

	// Bottom left corner
	glTexCoord2f(0.0, 0.0); glVertex2f(x, y - height);
	glTexCoord2f(0.0, 0.5); glVertex2f(x, y - height + windowborderh);
	glTexCoord2f(0.5, 0.5); glVertex2f(x + windowborderw, y - height + windowborderh);
	glTexCoord2f(0.5, 0.0); glVertex2f(x + windowborderw, y - height);

	// Bottom right corner
	glTexCoord2f(0.5, 0.0); glVertex2f(x + width - windowborderw, y - height);
	glTexCoord2f(0.5, 0.5); glVertex2f(x + width - windowborderw, y - height + windowborderh);
	glTexCoord2f(1.0, 0.5); glVertex2f(x + width, y - height + windowborderh);
	glTexCoord2f(1.0, 0.0); glVertex2f(x + width, y - height);

	// Top Bar
	glTexCoord2f(0.5, 0.0); glVertex2f(x + windowborderw, y - (windowborderh << 1));
	glTexCoord2f(0.5, 1.0); glVertex2f(x + windowborderw, y);
	glTexCoord2f(0.5, 1.0); glVertex2f(x + width - windowborderw, y);
	glTexCoord2f(0.5, 0.0); glVertex2f(x + width - windowborderw, y - (windowborderh << 1));

	// Bottom Bar
	glTexCoord2f(0.5, 0.0); glVertex2f(x + windowborderw, y - height);
	glTexCoord2f(0.5, 0.5); glVertex2f(x + windowborderw, y - height + windowborderh);
	glTexCoord2f(0.5, 0.5); glVertex2f(x + width - windowborderw, y - height + windowborderh);
	glTexCoord2f(0.5, 0.0); glVertex2f(x + width - windowborderw, y - height);

	// Left Bar
	glTexCoord2f(0.0, 0.5); glVertex2f(x, y - (windowborderh << 1));
	glTexCoord2f(0.5, 0.5); glVertex2f(x + windowborderw, y - (windowborderh << 1));
	glTexCoord2f(0.5, 0.5); glVertex2f(x + windowborderw, y - height + windowborderh);
	glTexCoord2f(0.0, 0.5); glVertex2f(x, y - height + windowborderh);

	// Right Bar
	glTexCoord2f(0.5, 0.5); glVertex2f(x + width - windowborderw, y - (windowborderh << 1));
	glTexCoord2f(1.0, 0.5); glVertex2f(x + width, y - (windowborderh << 1));
	glTexCoord2f(1.0, 0.5); glVertex2f(x + width, y - height + windowborderh);
	glTexCoord2f(0.5, 0.5); glVertex2f(x + width - windowborderw, y - height + windowborderh);

	// Center contents
	glTexCoord2f(0.5, 0.5);
	glVertex2f(x + windowborderw, y - (windowborderh << 1));
	glVertex2f(x + width - windowborderw, y - (windowborderh << 1));
	glVertex2f(x + width - windowborderw, y - height + windowborderh);
	glVertex2f(x + windowborderw, y - height + windowborderh);

	glEnd();

	glDisable(GL_TEXTURE_2D);

	if (OverX(w, mousex, mousey))
	{
		xsize += 3;

		if(downx && draggingwin == w)
			glColor4ub(0, 0, 0, 64);
		else
			glColor4ub(0, 0, 0, 32);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_QUADS);

		glVertex2f(window->x + window->w - xsize - 5, window->y - xsize - 2);
		glVertex2f(window->x + window->w - xsize - 5, window->y - 1);
		glVertex2f(window->x + window->w - 4, window->y - 1);
		glVertex2f(window->x + window->w - 4, window->y - xsize - 2);

		glEnd();
		glDisable(GL_BLEND);

		xsize -= 3;
	}

	glColor3ub(windowclosecol.r, windowclosecol.g, windowclosecol.b);
	icons.DrawString("x", x + width - (windowborderw << 1), y - (windowborderh << 1) + 3, (windowborderh << 1) - 6);
	glColor3ub(255, 255, 255);
	normal.DrawString(title, x + 3, y - (windowborderh << 1) + 5, txtsmall);
}

bool GUI::OverX(int w, int x, int y)
{
	int xsize;
	GuiWindow* window;

	xsize = (windowborderh << 1) - 3;
	window = windows[w];
	
	if (x < window->x + window->w - xsize - 5)
		return false;

	if (x > window->x + window->w - 5)
		return false;

	if (y > window->y - 1)
		return false;

	if (y < window->y - xsize - 2)
		return false;
		
	return true;
}
