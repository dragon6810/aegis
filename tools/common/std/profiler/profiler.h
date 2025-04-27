#ifndef _profiler_h
#define _profiler_h

#include <time.h>

#include <std/list/list.h>

#define PROFILER_MAX_NAME 256

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

typedef struct profiler_el_s profiler_el_t;

struct profiler_el_s
{
    char name[PROFILER_MAX_NAME];
    struct timespec start;
};

LIST_TYPE(profiler_el_t, list_profiler_el)

/*
 * ================================
 *      VARIABLE DEFENITIONS
 * ================================
*/

extern list_profiler_el_t profiler_stack;

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void profiler_setup(void);
void profiler_push(const char* name);
void profiler_pop(void);

#endif