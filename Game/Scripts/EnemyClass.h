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

EnemyClass::EnemyClass() : stunned(false), timeStunned(0.0f)
{
}

inline void EnemyClass::SetStunnedTime(float newTime)
{
	stunned = true;
	timeStunned = newTime;
}