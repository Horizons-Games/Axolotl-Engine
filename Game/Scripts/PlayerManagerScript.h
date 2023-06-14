#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum PowerUpType;

class PlayerManagerScript : public Script
{
public:
	PlayerManagerScript();
	~PlayerManagerScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	bool SavePowerUp(PowerUpType type);
	void UsePowerUp();
	void DropPowerUp();

	const PowerUpType GetPowerUpType();

private:
	float counterPowerUp;
	PowerUpType activePowerUp;
};