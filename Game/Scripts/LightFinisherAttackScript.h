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

	void ThrowStunItem(bool isMelee);
	bool IsAttacking();
	

private:
	GameObject* bulletPrefab;
	float bulletVelocity;	
	float stunTime;
	float currentCooldown;
	float cooldown;
	bool isActivated;

	Scene* loadedScene;
	EntityDetection* enemyDetection;
};
