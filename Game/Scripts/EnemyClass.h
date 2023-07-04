#pragma once

class EnemyClass
{
public:
	EnemyClass();
	~EnemyClass() = default;

	void SetStunnedTime(float newTime);

protected:
	bool stunned;
	float timeStunned;
};