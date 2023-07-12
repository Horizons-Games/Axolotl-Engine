#include "EnemyClass.h"

EnemyClass::EnemyClass() : stunned(false), timeStunned(0.0f)
{
}

EnemyClass::~EnemyClass()
{
}

void EnemyClass::SetStunnedTime(float newTime)
{
	stunned = true;
	timeStunned = newTime;
}