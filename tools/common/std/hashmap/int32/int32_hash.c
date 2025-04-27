#include <std/hashmap/hashmap.h>

#include <std/assert/assert.h>

unsigned long int hashmap_int32_hash(void* data)
{
    assert(data);

    return *((int*) data);
}
