#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentParticleSystem;
class GameObject;

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
	
private:

	float currentHealth;
	float maxHealth;
	bool isImmortal;
	GameObject* enemyParticleSystem;

	ComponentAnimation* componentAnimation;
	ComponentParticleSystem* componentParticleSystem;

	MeshEffect* meshEffect;
	float hitEffectDuration;
	bool hasTakenDamage;

	BixAttackScript* attackScript;
};
