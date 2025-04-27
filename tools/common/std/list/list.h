#ifndef _list_h
#define _list_h

/*
 * ================================
 *             MACROS
 * ================================
*/

#define LIST_TYPE(type, name) typedef struct name##_s \
{                               \
    unsigned long int size;     \
    unsigned long int buffsize; \
    unsigned long int elsize;   \
    type* data;                 \
} name##_t;

#define LIST_INITIALIZE(list) (list_initialize((list_template_t*)&(list), sizeof(typeof(*((list).data)))))

#define LIST_RESIZE(list, size) (list_resize((list_template_t*)&(list), (unsigned long int) size))

#define LIST_PUSH(list, value) list_push((list_template_t*)&(list), &value)

#define LIST_POP(list, out) (list_pop((list_template_t*)&(list), out))

#define LIST_REMOVE(list, index) (list_remove((list_template_t*)&(list), (unsigned long int) index));

#define LIST_REMOVERANGE(list, start, end) (list_removerange((list_template_t*)&(list), (unsigned long int) start, (unsigned long int) end))

#define LIST_INSERTLIST(list, sublist, where) (list_insertlist((list_template_t*)&(list), (list_template_t*)&(sublist), (unsigned long int) where))

#define LIST_FROMSPAN(new, list, start, end) (list_fromspan((list_template_t*)&(new), (list_template_t*)&(list), (unsigned long int) start, (unsigned long int) end))

#define LIST_COPY(new, old) (list_copy((list_template_t*)&(new), (list_template_t*)&(old)))

#define LIST_SHUFFLE(out, list) (list_shuffle((list_template_t*)&(out), (list_template_t*)&(list)))

#define LIST_FREE(list) (list_free((list_template_t*)&(list)))

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct list_template_s list_template_t;

struct list_template_s
{
    unsigned long int size;
    unsigned long int buffsize;
    unsigned long int elsize;
    void *data;
};

/*
 * ================================
 *      VARIABLE DEFENITIONS
 * ================================
*/

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void  list_initialize(list_template_t* list, unsigned long int elsize);
void  list_resize(list_template_t* list, unsigned long int size);
void* list_push(list_template_t* list, void* element);
void  list_pop(list_template_t* list, void* element);
void  list_remove(list_template_t* list, unsigned long int index);
void  list_removerange(list_template_t* list, unsigned long int start, unsigned long int end);
void  list_insertlist(list_template_t* list, list_template_t* sublist, unsigned long int where);
void  list_fromspan(list_template_t* new, list_template_t* list, unsigned long int start, unsigned long int end);
void  list_copy(list_template_t* new, list_template_t* old);
void  list_shuffle(list_template_t* out, list_template_t* list);
void  list_free(list_template_t* list);

#endif
