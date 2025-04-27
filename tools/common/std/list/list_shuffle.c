#include <std/list/list.h>

#include <std/assert/assert.h>
#include <stdlib.h>
#include <string.h>

#include <std/random/random.h>

static void list_shuffle_swap(void* a, void* b, unsigned long int size)
{
    void* temp;

    assert(a);
    assert(b);
    assert(size);

    if(a == b)
        return;

    temp = malloc(size);

    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);

    free(temp);
}

void list_shuffle(list_template_t* out, list_template_t* list)
{
    unsigned long int i, j;

    assert(out);
    assert(list);

    if(out != list)
        list_copy(out, list);

    list = out;

    for(i=list->size; i>0; i--)
    {
        j = random_int(0, i+1);
        list_shuffle_swap(list->data + list->elsize * i, list->data + list->elsize * j, list->elsize);
    }
}