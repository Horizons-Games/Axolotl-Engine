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

protected:
	bool stunned;
	float timeStunned;
	GameObject* particleGO;
	ComponentParticleSystem* particleTarget;
};