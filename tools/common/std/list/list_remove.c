#include <std/list/list.h>

#include <string.h>

#include <std/assert/assert.h>

void list_remove(list_template_t* list, unsigned long int index)
{
    void *before, *after;

    assert(list);
    assert(list->data);
    assert(index < list->size);

    before = malloc(index * list->elsize);
    after = malloc((list->size - index - 1) * list->elsize);
    memcpy(before, list->data, index * list->elsize);
    memcpy(after, list->data + (index + 1) * list->elsize, (list->size - index - 1) * list->elsize);    
    list->size--;

    if(list->buffsize >> 1 >= list->size * list->elsize)
    {
        list->buffsize >>= 1;
        free(list->data);
        list->data = malloc(list->buffsize);
    }

    memcpy(list->data, before, index * list->elsize);
    memcpy(list->data + index * list->elsize, after, (list->size - index) * list->elsize);

    free(after);
    free(before);
}