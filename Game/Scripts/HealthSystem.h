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
	
	float GetCurrentHealth() const;
	bool EntityIsAlive() const;

	float GetMaxHealth() const;
	void SetMaxHealth(float maxHealth);

	bool GetIsImmortal() const;
	void SetIsImmortal(bool isImmortal);
	
private:
	float currentHealth;
	float maxHealth;
	bool isImmortal;

	ComponentAnimation* componentAnimation;
};
