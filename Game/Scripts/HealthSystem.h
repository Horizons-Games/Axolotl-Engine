#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "vector"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentAnimation;
class ComponentMeshRenderer;
class ComponentParticleSystem;
class GameObject;

class BixAttackScript;

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
	void FillMeshes(GameObject* parent);

	void EffectDiscard();
	void EffectColor();
	void ClearEffect();

	float currentHealth;
	float maxHealth;
	bool isImmortal;
	GameObject* enemyParticleSystem;

	ComponentAnimation* componentAnimation;
	ComponentParticleSystem* componentParticleSystem;

	std::vector<ComponentMeshRenderer*> meshes;
	float hitEffectDuration;
	bool hasTakenDamage;

	BixAttackScript* attackScript;
};
