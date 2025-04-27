#include <std/hashmap/string/string.h>

#include <string.h>

#include <std/assert/assert.h>

unsigned long int hashmap_string_hash(void* data)
{
    unsigned int i;

    char *str;
    unsigned int stringlen;
    unsigned int h, c1, c2, r1, r2, m, n;
    unsigned int k, k1;

    assert(data);

    str = *((char**) data);
    assert(str);

    h = 0x9747b28c;
    c1 = 0xcc9e2d51;
    c2 = 0x1b873593;
    r1 = 15;
    r2 = 13;
    m = 5;
    n = 0xe6546b64;

    stringlen = strlen(str);
    for(i=0; i<stringlen/sizeof(unsigned int); i++)
    {
        k = str[i*sizeof(unsigned int)];
        
        k *=  c1;
        k  = (k << r1) | (k >> (32 - r1));
        k *=  c2;
        h ^=  k;
        h  = (h << r2) | (h >> (32 - r2));
        h  =  h * m + n;
    }

    k1 = 0;
    str += (stringlen & ~3);
    switch(stringlen & 3)
    {
        case 3:
            k1 ^= str[2] << 16;
        case 2:
            k1 ^= str[1] << 8;
        case 1:
            k1 ^= str[0];

            k1 *= c1;
            k1 = (k1 << r1) | (k1 >> (32 - r1));
            k1 *= c2;
            h ^= k1;
    }

    h ^= stringlen;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}
