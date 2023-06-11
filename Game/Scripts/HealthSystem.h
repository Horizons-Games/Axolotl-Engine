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

	float GetMaxHealth() const;
	void SetMaxHealth(float maxHealth);

	bool GetIsImmortal();
	void SetIsImmortal(bool isImmortal);
	

private:
	float currentHealth;
	float maxHealth;
	bool isImmortal;
	bool dead;

	ComponentAnimation* componentAnimation;

	std::string loseSceneName;
};



