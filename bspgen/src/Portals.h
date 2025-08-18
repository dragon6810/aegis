#pragma once

#include "Bsp.h"

void Portalize(model_t* mdl);
// only do this on world model
// finds and deletes all unused uncut faces
void FillModel(model_t* mdl);