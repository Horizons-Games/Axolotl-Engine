#pragma once

#include "Scripting\Script.h"

class GameObject;
class ComponentParticleSystem;

class EnemyClass : public Script
{
public:
	EnemyClass();
	~EnemyClass();

	void Init() override;

	void VisualTarget(bool activate);

	void SetStunnedTime(float newTime);

	bool IsSpawnedEnemy() const;

protected:
	bool stunned;
	float timeStunned;

	GameObject* particleTargetGO;
	ComponentParticleSystem* particleTarget;

	// This is used for when a certain enemy is spawn from nothing 
	// instead of being placed in the scene from the beginning
	bool isSpawnedEnemy;
};