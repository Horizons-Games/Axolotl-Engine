#pragma once

class EnemyClass
{
public:
	EnemyClass();
	~EnemyClass();

	void SetStunnedTime(float newTime);

protected:
	bool stunned;
	float timeStunned;
};