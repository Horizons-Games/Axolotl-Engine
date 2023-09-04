#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentParticleSystem;

class BixAttackScript;
class MeshEffect;


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
	
	void SetDeathCallback(std::function<void(void)>&& callDeath);

private:

	float currentHealth;
	float maxHealth;
	bool isImmortal;
	bool damageTaken;
	GameObject* enemyParticleSystem;

	ComponentAnimation* componentAnimation;
	ComponentParticleSystem* componentParticleSystem;

	MeshEffect* meshEffect;

	BixAttackScript* attackScript;

	std::function<void(void)> deathCallback;
};
