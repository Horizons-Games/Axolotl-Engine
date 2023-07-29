#pragma once
#include "Script.h"

class Scene;

class LightFinisherAttackScript : public Script
{

public:
	LightFinisherAttackScript();
	~LightFinisherAttackScript() override = default;

	void Start() override;

	void PerformAttack();

private:
	GameObject* bulletPrefab;
	float bulletVelocity;

	Scene* loadedScene;
};
