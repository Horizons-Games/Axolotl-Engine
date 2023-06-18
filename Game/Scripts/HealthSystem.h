#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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
	void IncreaseDefense(float increaseDefense);
	
	float GetCurrentHealth() const;

	bool EntityIsAlive() const;

	void PlayerDeath();

private:
	float currentHealth;
	float maxHealth;
	float defense;
	bool dead;

	ComponentAnimation* componentAnimation;

	std::string loseSceneName;
};