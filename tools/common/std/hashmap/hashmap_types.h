#ifndef _hashmap_types_h
#define _hashmap_types_h

#include <std/hashmap/hashmap.h>
#include <std/hashmap/string/string.h>
#include <std/hashmap/int32/int32.h>
#include <std/hashmap/int64/int64.h>

HASHMAP_TYPE_DECL(char*, long int, string_int64)

#endif