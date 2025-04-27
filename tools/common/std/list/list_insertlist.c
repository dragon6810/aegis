#include <std/list/list.h>

#include <string.h>

#include <std/assert/assert.h>

void list_insertlist(list_template_t* list, list_template_t* sublist, unsigned long int where)
{
    unsigned long int newsize;
    void *datacpy;

    assert(list);
    assert(sublist);
    assert(list != sublist);
    assert(list->elsize == sublist->elsize);
    assert(where <= list->size);

    datacpy = malloc(list->size * list->elsize);
    memcpy(datacpy, list->data, list->size * list->elsize);

    newsize = list->size + sublist->size;
    if(list->buffsize < newsize * list->elsize || list->buffsize == 0)
    {
        if(list->buffsize == 0)
            list->buffsize = 1;

        while(list->buffsize < newsize * list->elsize)
            list->buffsize <<= 1;

        free(list->data);
        list->data = malloc(list->buffsize);
    }

    memcpy(list->data, datacpy, where * list->elsize);
    memcpy(list->data + where * list->elsize, sublist->data, sublist->size * list->elsize);
    memcpy(list->data + (where + sublist->size) * list->elsize, datacpy + where * list->elsize, (list->size - where) * list->elsize);

    list->size = newsize;

    free(datacpy);
}