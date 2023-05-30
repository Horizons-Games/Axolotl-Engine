#pragma once

#include "Scripting\Script.h"

class ComponentAnimation;

class HealthSystem : public Script
{
public:
	HealthSystem();
	~HealthSystem() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void TakeDamage(float damage);
	void HealLife(float amountHealed);

	bool EntityIsAlive() const;

private:
	float currentHealth;
	float maxHealth;

	ComponentAnimation* componentAnimation;
};