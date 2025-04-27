#include <std/list/list.h>

#include <std/assert/assert.h>
#include <stdlib.h>
#include <string.h>

void list_resize(list_template_t* list, unsigned long int size)
{
    unsigned long int pad;

    assert(list->elsize);
    
    if(!list->data)
    {
        list->size = size;
        list->buffsize = 1;
        while(list->buffsize < list->size * list->elsize)
            list->buffsize <<= 1;

        list->data = calloc(1, list->buffsize);
        return;
    }

    pad = 0;
    if(size > list->size)
        pad = (size - list->size) * list->elsize;

    list->size = size;
    list->buffsize = size * list->elsize;
    list->data = realloc(list->data, list->buffsize);
    
    if(!pad)
        return;

    memset(list->data + list->size * list->elsize - pad, 0, pad);
}
