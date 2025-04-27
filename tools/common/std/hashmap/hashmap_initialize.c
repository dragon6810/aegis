#include <std/hashmap/hashmap.h>

#include <string.h>

#include <std/assert/assert.h>


void hashmap_initialize(hashmap_template_t* hashmap, unsigned long int (*hash)(void*),  bool (*cmp)(void*, void*), unsigned long int nbuckets, unsigned long int keysize, unsigned long int valsize, void (*freekey)(void*), void (*freeval)(void*), void  (*copykey)(void*, void*), void  (*copyval)(void*, void*))
{
    assert(hashmap);
    assert(hash);

    memset(hashmap, 0, sizeof(hashmap_template_t));

    hashmap->nbuckets = nbuckets;
    hashmap->buckets = calloc(hashmap->nbuckets, sizeof(hashmap_bucket_template_t));
    hashmap->keysize = keysize;
    hashmap->valsize = valsize;
    hashmap->hash = hash;
    hashmap->cmp = cmp;
    hashmap->freekey = freekey;
    hashmap->freeval = freeval;
    hashmap->copykey = copykey;
    hashmap->copyval = copyval;
}
