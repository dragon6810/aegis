#include "MonsterBarney.h"

MonsterBarney::MonsterBarney(BSPMap& map) : MonsterEntity(map)
{

}

void MonsterBarney::Init()
{
	model.map = map;
	model.Load("valve/models/barney.mdl");
	model.SetPosition(position.x, position.y, position.z);
	model.startseq(0);
	model.Say("barney/ba_bring.wav");
}

void MonsterBarney::Think(float deltatime)
{
	model.Tick();
}