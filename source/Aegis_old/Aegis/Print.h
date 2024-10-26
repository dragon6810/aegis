#pragma once

#include <string>
#include <iostream>

#define MAX_PRINT_LEN 1024

class Print
{
public:
	// MUST BE CALLED WITH A FINAL MESSAGE INCLUDING FORMATTING WITH A LENGTH LESS THAN 1024
	template <typename... Args>
	static void Aegis_Warning(const char* format, Args... args);
};

template <typename... Args>
void Print::Aegis_Warning(const char* format, Args... args)
{
    if (strlen(format) > MAX_PRINT_LEN)
        return;

    char out[MAX_PRINT_LEN + 1];

    sprintf(out, format, args...);

    if (strlen(out) > MAX_PRINT_LEN) // Uh oh. This might be a buffer overrun. Truly disgusting code. If the game crashes, my bad.
        return;

    printf("*WARNING*: %s", out);
}
