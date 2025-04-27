#ifndef _textutils_h
#define _textutils_h

#include <stdbool.h>

char* textutils_remove(char* str, int start, int end);
char* textutils_insert(char* str, const char* insert, int pos);
bool textutils_ischarcancelled(char* str, char* c);

#endif