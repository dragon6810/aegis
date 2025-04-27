#include <std/random/random.h>

#include <std/assert/assert.h>
#include <stdlib.h>

int random_int(int min, int max)
{
    unsigned int us;
    int s;

    assert(max >= min);

    if(max == min)
        return min;

    us = (unsigned int) rand() % (max - min);
    s = (int) us + min;

    return s;
}