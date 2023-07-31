#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentParticleSystem;

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
	float GetMaxHealth() const;

	bool EntityIsAlive() const;

	bool IsImmortal() const;
	void SetIsImmortal(bool isImmortal);
	
private:
	float currentHealth;
	float maxHealth;
	bool isImmortal;
	GameObject* enemyParticleSystem;

	ComponentAnimation* componentAnimation;
	ComponentParticleSystem* componentParticleSystem;
};

inline void HealthSystem::HealLife(float amountHealed)
{
	currentHealth = std::min(currentHealth + amountHealed, maxHealth);
}

inline bool HealthSystem::EntityIsAlive() const
{
	return currentHealth > 0;
}

inline float HealthSystem::GetMaxHealth() const
{
	return maxHealth;
}

inline bool HealthSystem::IsImmortal() const
{
	return isImmortal;
}

inline void HealthSystem::SetIsImmortal(bool isImmortal)
{
	this->isImmortal = isImmortal;
}

inline float HealthSystem::GetCurrentHealth() const
{
	return currentHealth;
}
