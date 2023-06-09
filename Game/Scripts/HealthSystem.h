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
	void IncreaseDefense(float increaseDefense);

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

inline void HealthSystem::IncreaseDefense(float increaseDefense)
{
	defense += increaseDefense;
	ENGINE_LOG("Defense increased");
}