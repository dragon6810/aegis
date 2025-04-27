#include <std/hashmap/hashmap.h>

#include <std/assert/assert.h>
#include <stdio.h>

void* hashmap_fetch(hashmap_template_t* hashmap, void* key)
{
    hashmap_bucketitem_template_t *curitem;

    unsigned long int hash, index;
    hashmap_bucket_template_t *bucket;

    assert(hashmap);
    assert(key);
    assert(hashmap->buckets);
    assert(hashmap->nbuckets);

    hash = hashmap->hash(key);
    index = hash % hashmap->nbuckets;
    bucket = &hashmap->buckets[index];

    if(!bucket->items)
        return NULL;

    for(curitem=bucket->items; ; curitem=curitem->next)
    {
        if(!curitem)
            break;

        if(curitem->hash != hash)
            continue;

        if(!hashmap->cmp(key, curitem->key))
            continue;

        return curitem->val;
    }

    return NULL;
}
