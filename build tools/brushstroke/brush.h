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
void GenBB(brushdef_t* brsh);
void Optimize(brushdef_t* brsh, entitydef_t* set);

boolean PolyInsideBrush(polynode_t* p, brushdef_t* brsh);
void CullPoly(polynode_t* p, brushdef_t* owner);
void CullPlane(planedef_t* p, brushdef_t* owner);

vnode_t* GetPoints(brushdef_t *brsh);

#endif /* brush_h */
