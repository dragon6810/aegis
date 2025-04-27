#ifndef _PARSELIB_H
#define _PARSELIB_H

#include <stdbool.h>

typedef struct tokenstate_s
{
    char *buf;
    char *pos;
    char *token;
} tokenstate_t;

void parselib_start(char *buf, tokenstate_t *tokenstate);
bool parselib_nexttoken(tokenstate_t *tokenstate);
void parselib_stop(tokenstate_t *tokenstate);

#endif