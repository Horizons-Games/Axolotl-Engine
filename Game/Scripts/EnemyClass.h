#pragma once

#include "Scripting\Script.h"

class EnemyClass : public Script
{
public:
	EnemyClass();
	~EnemyClass();

	void SetStunnedTime(float newTime);

protected:
	bool stunned;
	float timeStunned;
};