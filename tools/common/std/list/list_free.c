#include <std/list/list.h>

#include <std/assert/assert.h>
#include <stdlib.h>
#include <string.h>

void list_free(list_template_t* list)
{
    assert(list);

    if(list->data)
        free(list->data);
    memset(list, 0, sizeof(list_template_t));
}
