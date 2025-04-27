#include <std/hashmap/hashmap.h>

#include <std/assert/assert.h>

bool hashmap_remove(hashmap_template_t* hashmap, void* key)
{
    unsigned long int hash, index;
    hashmap_bucket_template_t *bucket;
    hashmap_bucketitem_template_t *item, *last;

    assert(hashmap);
    assert(hashmap->buckets);
    assert(hashmap->nbuckets);
    assert(key);

    hash = hashmap->hash(key);
    index = hash % hashmap->nbuckets;
    bucket = &hashmap->buckets[index];
    
    if(!bucket->items)
        return false;

    last = 0;
    item = bucket->items;
    while(item->next)
    {
        if(item->hash == hash && hashmap->cmp(item->key, key))
            break;

        last = item;
        item = item->next;
    }

    if(item->hash != hash || !hashmap->cmp(item->key, key))
        return false;

    if(hashmap->freekey)
        hashmap->freekey(item->key);
    if(hashmap->freeval)
        hashmap->freeval(item->val);
    free(item->key);
    free(item->val);
    if(last)
        last->next = item->next;
    free(item);

    return true;
}