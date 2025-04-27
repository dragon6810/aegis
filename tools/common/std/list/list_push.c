#include <std/list/list.h>

#include <std/assert/assert.h>
#include <stdlib.h>
#include <string.h>

void* list_push(list_template_t* list, void* element)
{
    assert(list);
    assert(list->elsize);
    assert(element);

    if(!list->data)
    {
        list->size = 1;
        list->buffsize = list->elsize;
        list->data = malloc(list->elsize);
        memcpy(list->data, element, list->elsize);
        return list->data;
    }

    list->size++;
    if(list->buffsize < list->size * list->elsize)
    {
        list->buffsize <<= 1;
        list->data = realloc(list->data, list->buffsize);
    }

    memcpy(list->data + (list->size - 1) * list->elsize, element, list->elsize);
    return list->data + (list->size - 1) * list->elsize;
}
