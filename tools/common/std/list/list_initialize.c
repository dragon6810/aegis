#include <std/list/list.h>

#include <std/assert/assert.h>
#include <string.h>

void list_initialize(list_template_t* list, unsigned long int elsize)
{
    assert(elsize);

    memset(list, 0, sizeof(list_template_t));
    list->elsize = elsize;
}
