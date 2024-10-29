//
//  textfile.h
//  Brushstroke
//
//  Created by Henry Dunn on 8/22/24.
//

#ifndef textfile_h
#define textfile_h

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"

extern char token[64];
extern char* script, * scriptp, * scriptend;
extern int scriptline;

void GetLine(char** out, FILE* ptr);

void LoadScript(char* filename);
boolean NextToken();
boolean TokenAvailable();

#endif /* textfile_h */
