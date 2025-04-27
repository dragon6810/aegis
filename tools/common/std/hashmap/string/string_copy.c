#include <std/hashmap/string/string.h>

#include <string.h>

#include <std/assert/assert.h>

void hashmap_string_copy(void* dst, void* src)
{
    char *strsrc;

    assert(dst);
    assert(src);

    strsrc = *((char**)src);

    *((char**)dst) = strdup(strsrc);
}
