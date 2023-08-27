#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentParticleSystem;
class ComponentRigidBody;
class ComponentTransform;
class ComponentMeshRenderer;
class ComponentLight;

enum class BoostOfEnergyStates 
{
	AIMING,
	PRESHOOTING,
	SHOOTING,
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
	ComponentParticleSystem* aimingParticle;
	ComponentParticleSystem* preshootingParticle;
	ComponentParticleSystem* shootingParticle;

	float attackDamage;

	ComponentRigidBody* rigidBody;
	ComponentMeshRenderer* mesh;
	ComponentTransform* target;
	ComponentLight* light;
};