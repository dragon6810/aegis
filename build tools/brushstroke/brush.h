//
//  brush.h
//  Brushstroke
//
//  Created by Henry Dunn on 8/24/24.
//

#ifndef brush_h
#define brush_h

#include <stdio.h>
#include <string.h>

#include "mapdefs.h"

void AddPlane(brushdef_t* brsh, planedef_t pl);
void GenPolys(brushdef_t* brsh);
void CutPolys(brushdef_t* brsh);

#endif /* brush_h */
