#pragma once

#include "Scripting\Script.h"

class HealthSystem : public Script
{
public:
	HealthSystem();
	~HealthSystem() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TakeDamage(float damage);
	void HealLife(float amountHealed);

private:
	float currentHealth;
	float maxHealth;
};