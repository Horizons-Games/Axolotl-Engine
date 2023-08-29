#pragma once

#include "Scripting\Script.h"

class GameObject;
class ComponentParticleSystem;

enum class EnemyTypes
{
	NONE,
	DRONE,
	VENOMITE,
	MINI_BOSS,
	FINAL_BOSS
};

class EnemyClass : public Script
{
public:
	EnemyClass();
	~EnemyClass();

	void Init() override;

	void VisualTarget(bool activate);

	virtual void SetStunnedTime(float newTime);
	float GetStunnedTime() const;

	bool IsSpawnedEnemy() const;

	bool NeedsToBeReset() const;
	void ActivateNeedsToBeReset();
	void DeactivateNeedsToBeReset();

	EnemyTypes GetEnemyType() const;

	virtual void SetReadyToDie() = 0;

protected:
	bool stunned;
	float timeStunned;

	GameObject* particleTargetGO;
	ComponentParticleSystem* particleTarget;

	// This is used for when a certain enemy is spawn from nothing 
	// instead of being placed in the scene from the beginning
	bool isSpawnedEnemy;
	bool needsToBeReset;

	EnemyTypes enemyType;
};