#include <std/hashmap/int32/int32.h>

#include <std/assert/assert.h>

bool hashmap_int64_cmp(void* a, void* b)
{
    assert(a);
    assert(b);

    return *((long int*) a) == *((long int*) b);
}
