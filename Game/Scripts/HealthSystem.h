#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentParticleSystem;

class PlayerAttackScript;
class MeshEffect;
class PlayerManagerScript;

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
	void SetMaxHealth(float maxHealth);

	bool EntityIsAlive() const;

	bool IsImmortal() const;
	void SetIsImmortal(bool isImmortal);
	void SetImmortalTimer(float duration);
	
	void SetDeathCallback(std::function<void(void)>&& callDeath);

	MeshEffect* GetMeshEffect() const;

private:

	float currentHealth;
	float maxHealth;
	bool isImmortal;
	bool damageTaken;
	float immortalTimer;
	GameObject* enemyParticleSystem;

	ComponentAnimation* componentAnimation;
	ComponentParticleSystem* componentParticleSystem;

	PlayerAttackScript* attackScript;
	MeshEffect* meshEffect;
	PlayerManagerScript* playerManager;

	std::function<void(void)> deathCallback;
};
