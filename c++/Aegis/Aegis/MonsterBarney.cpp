#include "MonsterBarney.h"

MonsterBarney::MonsterBarney(BSPMap& map) : MonsterEntity(map)
{

}

void MonsterBarney::Init()
{
	model.Load("valve/models/barney.mdl");
	model.startseq(0);
}