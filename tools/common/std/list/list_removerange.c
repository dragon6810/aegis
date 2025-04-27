#include <std/list/list.h>

#include <stdlib.h>
#include <string.h>

#include <std/assert/assert.h>

void list_removerange(list_template_t* list, unsigned long int start, unsigned long int end)
{
    void *listcopy;

    assert(list);
    assert(start < list->size);
    assert(end <= list->size);
    assert(start < end);

    listcopy = malloc(list->size * list->elsize);
    memcpy(listcopy, list->data, list->size * list->elsize);

    list->size -= end - start;
    if(list->buffsize >> 1 > list->size * list->elsize)
    {
        while(list->buffsize >> 1 > list->size * list->elsize)
            list->buffsize >>= 1;

        free(list->data);
        list->data = malloc(list->buffsize);
    }

    memcpy(list->data, listcopy, start * list->elsize);
    memcpy(list->data + start * list->elsize, listcopy + end * list->elsize, (list->size - start) * list->elsize);

    free(listcopy);
}