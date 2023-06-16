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
	float GetCurrentHealth() const;

	bool EntityIsAlive() const;

	void PlayerDeath();

private:
	float currentHealth;
	float maxHealth;
	bool dead;

	ComponentAnimation* componentAnimation;

	std::string loseSceneName;
};