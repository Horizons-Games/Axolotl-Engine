#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

enum class PowerUpType;

class PowerUpsManagerScript : public Script
{
public:
	PowerUpsManagerScript();
	~PowerUpsManagerScript() override = default;

	void Update(float deltaTime) override;

	bool SavePowerUp(const PowerUpType& type);
	void DropPowerUp();
	void UsePowerUp();

	const PowerUpType& GetSavedPowerUpType() const;
	const PowerUpType& GetActivePowerUpType() const;

private:
	void DeactivateCurrentPowerUp();

	float amountHealed;
	float attackIncrease;
	float defenseIncrease;
	float speedIncrease;

	float maxPowerUpTimer;
	float currentPowerUpTimer;

	GameObject* player;

	// To be changed to Player Manager
	PowerUpType activePowerUp;
	PowerUpType savedPowerUp;
};