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

	virtual void SetReadyToDie(float damage) = 0;

protected:
	bool stunned;
	float timeStunned;
	GameObject* particleTargetGO;
	ComponentParticleSystem* particleTarget;
};