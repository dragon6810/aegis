#include <std/list/list.h>

#include <std/assert/assert.h>
#include <stdlib.h>
#include <string.h>

void list_fromspan(list_template_t* new, list_template_t* list, unsigned long int start, unsigned long int end)
{
    assert(new);
    assert(list);
    assert(end >= start);
    assert(start < list->size);
    assert(end <= list->size);

    new->elsize = list->elsize;

    if(end == start)
    {
        list->buffsize = list->size = 0;
        list->data = 0;
        return;
    }

    new->size = end - start;
    new->buffsize = 1;
    while(new->buffsize < new->size * new->elsize)
        new->buffsize <<= 1;

    new->data = malloc(new->buffsize);
    memcpy(new->data, list->data + start * list->elsize, (end - start) * list->elsize);
}