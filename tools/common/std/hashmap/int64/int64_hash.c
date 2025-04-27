#include <std/hashmap/hashmap.h>

#include <std/assert/assert.h>

unsigned long int hashmap_int64_hash(void* data)
{
    assert(data);

    return *((long int*) data);
}
