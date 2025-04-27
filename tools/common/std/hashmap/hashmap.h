#ifndef _hashmap_h
#define _hashmap_h

#include <stdbool.h>

/*
 * ================================
 *             MACROS
 * ================================
*/

#define HASHMAP_BUCKETS_SMALL  16
#define HASHMAP_BUCKETS_MEDIUM 64
#define HASHMAP_BUCKETS_LARGE  1024

#define HASHMAP_TYPE_DECL(keytype, valtype, name)              \
typedef struct hashmap_bucketitem_##name##_s                   \
{                                                              \
    struct hashmap_bucketitem_##name##_s *next;                \
    unsigned long int hash;                                    \
    keytype *key;                                              \
    valtype *val;                                              \
} hashmap_bucketitem_##name##_t;                               \
                                                               \
typedef struct hashmap_bucket_##name##_s                       \
{                                                              \
    hashmap_bucketitem_##name##_t *items;                      \
} hashmap_bucket_##name##_t;                                   \
                                                               \
typedef struct hashmap_##name##_s                              \
{                                                              \
    hashmap_bucket_##name##_t *buckets;                        \
    unsigned long int nbuckets;                                \
    unsigned long int keysize, valsize;                        \
                                                               \
    unsigned long int (*hash)(void*);                          \
    bool (*cmp)(void*, void*);                                 \
    void (*freekey)(void*);                                    \
    void (*freeval)(void*);                                    \
    void (*copykey)(void*, void*);                             \
    void (*copyval)(void*, void*);                             \
} hashmap_##name##_t;                                          \
                                                               \
extern unsigned long int (*hashmap_type_##name##_hash)(void*); \
extern bool (*hashmap_type_##name##_cmp)(void*, void*);        \
extern void (*hashmap_type_##name##_keyfree)(void*);           \
extern void (*hashmap_type_##name##_valfree)(void*);           \
extern void (*hashmap_type_##name##_keycopy)(void*, void*);    \
extern void (*hashmap_type_##name##_valcopy)(void*, void*);    

#define HASHMAP_TYPE_DEF(keyhash, keycmp, keyfree, valfree, keycopy, valcopy, name) \
unsigned long int (*hashmap_type_##name##_hash)(void*) = keyhash; \
bool (*hashmap_type_##name##_cmp)(void*, void*) = keycmp;         \
void (*hashmap_type_##name##_keyfree)(void*) = keyfree;           \
void (*hashmap_type_##name##_valfree)(void*) = valfree;           \
void (*hashmap_type_##name##_keycopy)(void*, void*) = keycopy;    \
void (*hashmap_type_##name##_valcopy)(void*, void*) = valcopy;   

#define HASHMAP_INITIALIZE(hashmap, nbuckets, type)    \
hashmap_initialize                                     \
(                                                      \
    (hashmap_template_t*)&(hashmap),                   \
    hashmap_type_##type##_hash,                        \
    hashmap_type_##type##_cmp,                         \
    nbuckets,                                          \
    sizeof(*((hashmap_bucketitem_##type##_t*)0)->key), \
    sizeof(*((hashmap_bucketitem_##type##_t*)0)->val), \
    hashmap_type_##type##_keyfree,                     \
    hashmap_type_##type##_valfree,                     \
    hashmap_type_##type##_keycopy,                     \
    hashmap_type_##type##_valcopy                      \
)

#define HASHMAP_FETCH(hashmap, key) hashmap_fetch((hashmap_template_t*)&(hashmap), (void*)&(key))

#define HASHMAP_SET(hashmap, key, val) hashmap_set((hashmap_template_t*)&(hashmap), (void*)&(key), (void*)&(val))

#define HASHMAP_REMOVE(hashmap, key) hashmap_remove((hashmap_template_t*)&(hashmap), (void*)&(key))

#define HASHMAP_FREE(hashmap) hashmap_free((hashmap_template_t*)&(hashmap))

#define HASHMAP_EXECFORALL(hashmap, routine) hashmap_execforall((hashmap_template_t*)&(hashmap), ((void(*)(void*))(routine)))

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct hashmap_template_s hashmap_template_t;
typedef struct hashmap_bucket_template_s hashmap_bucket_template_t;
typedef struct hashmap_bucketitem_template_s hashmap_bucketitem_template_t;

struct hashmap_template_s
{
    hashmap_bucket_template_t *buckets;
    unsigned long int nbuckets;
    unsigned long int keysize, valsize;

    unsigned long int (*hash)(void*);
    bool (*cmp)(void*, void*);
    void (*freekey)(void*);
    void (*freeval)(void*);
    void (*copykey)(void*, void*);
    void (*copyval)(void*, void*);
};

struct hashmap_bucket_template_s
{
    hashmap_bucketitem_template_t *items;
};

struct hashmap_bucketitem_template_s
{
    hashmap_bucketitem_template_t *next;
    unsigned long int hash;
    void *key, *val;
};

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void hashmap_initialize(hashmap_template_t* hashmap, unsigned long int (*hash)(void*),  bool (*cmp)(void*, void*), unsigned long int nbuckets, unsigned long int keysize, unsigned long int valsize, void (*freekey)(void*), void (*freeval)(void*), void  (*copykey)(void*, void*), void  (*copyval)(void*, void*));
void* hashmap_fetch(hashmap_template_t* hashmap, void* key);
void* hashmap_set(hashmap_template_t* hashmap, void* key, void* val);
bool hashmap_remove(hashmap_template_t* hashmap, void* key);
void hashmap_free(hashmap_template_t* hashmap);
void hashmap_execforall(hashmap_template_t* hashmap, void (*routine)(void*));

#endif
