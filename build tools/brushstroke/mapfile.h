//
//  mapfile.h
//  Brushstroke
//
//  Created by Henry Dunn on 8/22/24.
//

#ifndef mapfile_h
#define mapfile_h

#include <string.h>

#include "defs.h"
#include "textfile.h"
#include "mapdefs.h"
#include "brush.h"

extern int nhulls;

FILE* mapfile;
FILE** hullouts;

char* filename, *line;
extern int linenum;

extern entitydef_t *firstent, *lastent;

void ParseMap(char* name);
boolean ParseEntry();
entitydef_t* ParseEntity();
brushdef_t* ParseBrush();
void Finish();
void NextLine();

void Error(char* msg);

entitypair_t* AllocEntPair();
entitydef_t* AllocEntity();

#endif /* mapfile_h */
