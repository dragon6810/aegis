#include <std/hashmap/string/string.h>

#include <std/assert/assert.h>

void hashmap_string_free(void* data)
{
    char *str;

    assert(data);

    str = *((char**)data);
    assert(str);

    free(str);
}
