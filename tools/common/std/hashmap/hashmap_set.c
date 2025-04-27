#include <std/hashmap/hashmap.h>

#include <string.h>

#include <std/assert/assert.h>

void* hashmap_set(hashmap_template_t* hashmap, void* key, void* val)
{
    unsigned long int hash, index;
    hashmap_bucket_template_t *bucket;
    hashmap_bucketitem_template_t *item;

    assert(hashmap);
    assert(hashmap->buckets);
    assert(hashmap->nbuckets);
    assert(key);
    assert(val);

    hash = hashmap->hash(key);
    index = hash % hashmap->nbuckets;
    bucket = &hashmap->buckets[index];
    
    if(!bucket->items)
    {
        item = calloc(1, sizeof(hashmap_bucketitem_template_t));
        bucket->items = item;
    }
    else
    {
        item = bucket->items;
        while(item->next)
        {
            if(item->hash == hash && hashmap->cmp(item->key, key))
                break;

            item = item->next;
        }

        if(item->hash != hash || !hashmap->cmp(item->key, key))
        {
            item->next = calloc(1, sizeof(hashmap_bucketitem_template_t));
            item = item->next;
        }
    }

    item->hash = hash;
    if(!item->key)
    {
        item->key = malloc(hashmap->keysize);
        if(hashmap->copykey)
            hashmap->copykey(item->key, key);
        else
            memcpy(item->key, key, hashmap->keysize);
    }

    if(item->val)
    {
        if(hashmap->freeval)
            hashmap->freeval(item->val);
        free(item->val);
    }
    item->val = malloc(hashmap->valsize);
    if(hashmap->copyval)
        hashmap->copyval(item->val, val);
    else
        memcpy(item->val, val, hashmap->valsize);

    return item->val;
}
