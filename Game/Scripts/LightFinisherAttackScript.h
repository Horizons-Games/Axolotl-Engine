#pragma once
#include "Script.h"

class Scene;
class EntityDetection;

class LightFinisherAttackScript : public Script
{

public:
	LightFinisherAttackScript();
	~LightFinisherAttackScript() override = default;

	void Start() override;

	void ThrowStunItem();

private:
	GameObject* bulletPrefab;
	float stunTime;
	float currentCooldown;
	float cooldown;

	Scene* loadedScene;
	EntityDetection* enemyDetection;
};
