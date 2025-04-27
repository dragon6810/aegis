#include <std/hashmap/hashmap.h>
#include <std/hashmap/hashmap_types.h>

#include <stddef.h>

HASHMAP_TYPE_DEF(hashmap_string_hash, hashmap_string_cmp, hashmap_string_free, NULL, hashmap_string_copy, NULL, string_int64)