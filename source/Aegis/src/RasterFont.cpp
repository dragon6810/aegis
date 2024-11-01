#include "RasterFont.h"

int RasterFont::DrawChar(char c, int x, int y)
{
    int w, h;
    int u, v;

    w = widths[c];
    h = tex->height;

    u = offsets[c];
    v = 0;

    if(!w)
        return 0;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex->name);

    glBegin(GL_QUADS);
    glTexCoord2f(u + 0, v + 0); glVertex2f(x + 0, y + 0);
    glTexCoord2f(u + 0, v + h); glVertex2f(x + 0, y + h);
    glTexCoord2f(u + w, v + h); glVertex2f(x + w, y + h);
    glTexCoord2f(u + w, v + 0); glVertex2f(x + w, y + 0);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(0, 0);
    glTexCoord2f(0, tex->height); glVertex2f(0, 240);
    glTexCoord2f(tex->width, tex->height); glVertex2f(320, 240);
    glTexCoord2f(tex->width, tex->height); glVertex2f(320, 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    return w + 1;
}

int RasterFont::DrawText(std::string name, int x, int y)
{
    DrawChar(name[0], x, y);
}