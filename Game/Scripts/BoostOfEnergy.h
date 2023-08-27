#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentParticleSystem;
class ComponentRigidBody;
class ComponentTransform;
class ComponentLight;

enum class BoostOfEnergyStates 
{
	AIMING,
	PRESHOOTING,
	SHOOTING,
	DEACTIVATING,
	RECHARGING,
	RECHARGED
};

class BoostOfEnergy : public Script
{
public:
	BoostOfEnergy();
	~BoostOfEnergy() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void PerformAttack();
	bool IsAttackAvailable();

private:
	void PreShootLaser();
	void ShootLaser();

	BoostOfEnergyStates attackState;

	float cooldownDuration;
	float cooldownTimer;

	float aimingDuration;
	float aimingTimer;
	float preshootingDuration;
	float preshootingTimer;
	float shootingDuration;
	float shootingTimer;
	float deactivationDuration;
	float deactivationTimer;
	ComponentParticleSystem* aimingParticle;
	ComponentParticleSystem* preshootingParticle;
	ComponentParticleSystem* shootingParticle;
	ComponentParticleSystem* deactivatingParticle;


	float attackDamage;

	ComponentRigidBody* rigidBody;
	ComponentTransform* mesh;
	ComponentTransform* target;
	ComponentLight* light;
};