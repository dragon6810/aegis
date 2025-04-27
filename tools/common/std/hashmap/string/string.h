#ifndef _hashmap_string_h
#define _hashmap_string_h

#include <stdbool.h>

unsigned long int hashmap_string_hash(void* data);
bool hashmap_string_cmp(void* a, void* b);
void hashmap_string_free(void* data);
void hashmap_string_copy(void* dst, void* src);

#endif
