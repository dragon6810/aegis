#include <std/hashmap/string/string.h>

#include <string.h>

#include <std/assert/assert.h>

bool hashmap_string_cmp(void* a, void* b)
{
    char *stra, *strb;

    assert(a);
    assert(b);

    stra = *((char**) a);
    strb = *((char**) b);

    return !strcmp(stra, strb);
}
