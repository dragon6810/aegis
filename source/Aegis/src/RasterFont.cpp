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

    if(c == ' ')
        return w + 1;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex->name);

    glBegin(GL_QUADS);
    glTexCoord2f((float) (u + 0) / (float) tex->width, (float) (v + 0) / (float) tex->height); glVertex2f(x + 0, y + 0);
    glTexCoord2f((float) (u + 0) / (float) tex->width, (float) (v + h) / (float) tex->height); glVertex2f(x + 0, y + h);
    glTexCoord2f((float) (u + w) / (float) tex->width, (float) (v + h) / (float) tex->height); glVertex2f(x + w, y + h);
    glTexCoord2f(((float) u + w) / (float) tex->width, ((float) v + 0) / (float) tex->height); glVertex2f(x + w, y + 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    return w + 1;
}

int RasterFont::DrawText(std::string name, int x, int y)
{
    int curx;
    int cury;
    char* curc;

    for(curc = &name[0], curx = x, cury = y; curc < &name[name.size()]; curc++)
    {
        if(*curc == '\n')
        {
            cury -= tex->height;
            curx = x;
            continue;
        }

        curx += DrawChar(*curc, curx, cury);
    }
}