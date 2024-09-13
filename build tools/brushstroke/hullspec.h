//
//  hullspec.h
//  Brushstroke
//
//  Created by Henry Dunn on 9/12/24.
//

#ifndef hullspec_h
#define hullspec_h

#define NHULLS 4

#include <stdio.h>
#include "vector.h"

extern vec3_t hmins[NHULLS];
extern vec3_t hmaxs[NHULLS];

FILE* hullptr;

void ReadHullSpec(char* path);

#endif /* hullspec_h */
