#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;

class EnemyManagerScript : public Script
{
public:
	EnemyManagerScript();
	~EnemyManagerScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	GameObject* RequestPowerUp();

private:
	GameObject* powerUp;
};