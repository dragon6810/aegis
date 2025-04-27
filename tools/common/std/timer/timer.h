#ifndef _timer_h
#define _timer_h

#include <time.h>

/*
 * ================================
 *        TYPES DEFENITIONS
 * ================================
*/

/*
 * ================================
 *      VARIABLE DEFENITIONS
 * ================================
*/

extern float timer_elapsedms;

extern struct timespec timer_starttime;
extern struct timespec timer_endtime;

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void timer_begin(void);
float timer_end(void);

#endif
