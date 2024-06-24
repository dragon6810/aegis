#include "MonsterBarney.h"

MonsterBarney::MonsterBarney(BSPMap& map) : MonsterEntity(map)
{

}

void MonsterBarney::Init()
{
	model.map = map;
	model.Load("valve/models/barney.mdl");
	model.SetPosition(position.x, position.y, position.z);
	model.startseq(53);
}