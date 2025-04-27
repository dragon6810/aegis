#ifndef _random_h
#define _random_h

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

/*
 * ================================
 *  EXTERNAL ROUTINES DEFENITIONS
 * ================================
*/

void random_seed(void);

float random_float(float min, float max);
int random_int(int min, int max);

#endif
