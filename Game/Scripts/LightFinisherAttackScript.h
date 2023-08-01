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
	void Update(float deltaTime) override;

	bool PerformAttack();

private:
	GameObject* bulletPrefab;
	float bulletVelocity;	
	float stunTime;
	float currentCooldown;
	float cooldown;

	Scene* loadedScene;
	EntityDetection* enemyDetection;
	//GameObject* enemyDetectionObject;
};
