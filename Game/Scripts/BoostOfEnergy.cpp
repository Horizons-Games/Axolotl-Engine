#include "StdAfx.h"
#include "BoostOfEnergy.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentLight.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/SendTriggerCollision.h"

REGISTERCLASS(BoostOfEnergy);

BoostOfEnergy::BoostOfEnergy() : Script(), preshootingDuration(0.5f), preshootingTimer(0.0f),
aimingDuration(2.0f), aimingTimer(0.0f), aimingParticle(nullptr), preshootingParticle(nullptr), 
shootingParticle(nullptr), attackDamage(30.0f), damageZone(nullptr), target(nullptr),
attackState(BoostOfEnergyStates::RECHARGED), shootingDuration(1.0f), shootingTimer(0.0f),
cooldownDuration(5.0f), cooldownTimer(0.0f), mesh(nullptr), light(nullptr),
deactivationDuration(0.5f), deactivationTimer(0.0f), deactivatingParticle(nullptr),
isPlayerInDamageZone(false), damageFrequency(0.5f), lastDamageTime(0.0f), sendTriggerCollision(nullptr)
{
	REGISTER_FIELD(attackDamage, float);
	REGISTER_FIELD(damageFrequency, float);
	REGISTER_FIELD(cooldownDuration, float);
	REGISTER_FIELD(aimingDuration, float);
	REGISTER_FIELD(preshootingDuration, float);
	REGISTER_FIELD(shootingDuration, float);
	REGISTER_FIELD(aimingParticle, ComponentParticleSystem*);
	REGISTER_FIELD(preshootingParticle, ComponentParticleSystem*);
	REGISTER_FIELD(shootingParticle, ComponentParticleSystem*);
	REGISTER_FIELD(deactivatingParticle, ComponentParticleSystem*);
	REGISTER_FIELD(deactivationDuration, float);
	REGISTER_FIELD(mesh, ComponentTransform*);
	REGISTER_FIELD(light, ComponentLight*);
	REGISTER_FIELD(target, ComponentTransform*);
	REGISTER_FIELD(damageZone, ComponentRigidBody*);
}

void BoostOfEnergy::Start()
{
	mesh->GetOwner()->Disable();
	light->Disable();

	sendTriggerCollision = damageZone->GetOwner()->GetComponent<SendTriggerCollision>();
	sendTriggerCollision->SetEnterCollisionCallback(std::bind(&BoostOfEnergy::SetCollisionEnter, this, std::placeholders::_1));
	sendTriggerCollision->SetExitCollisionCallback(std::bind(&BoostOfEnergy::SetCollisionExit, this, std::placeholders::_1));

	lastDamageTime = damageFrequency;
}

void BoostOfEnergy::Update(float deltaTime)
{

	damageZone->UpdateRigidBody();

	switch(attackState)
	{
	case BoostOfEnergyStates::AIMING: //Preparing laser while aiming to the player
		aimingTimer += deltaTime;
		if (aimingTimer >= aimingDuration)
		{
			PreShootLaser();
		}
		break;

	case BoostOfEnergyStates::PRESHOOTING: //Stop aiming to the player (stays without moving) and charging laser
		preshootingTimer += deltaTime;
		if (preshootingTimer >= preshootingDuration)
		{
			ShootLaser();
		}
		break;

	case BoostOfEnergyStates::SHOOTING: //Shooting laser without moving for shootingDuration time

		lastDamageTime += deltaTime;
		if (isPlayerInDamageZone && (lastDamageTime >= damageFrequency))
		{
			lastDamageTime = 0.0f;
			target->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(attackDamage);
		}

		shootingTimer += deltaTime;
		if (shootingTimer >= shootingDuration)
		{
			DeactivateLaser();
		}
		break;

	case BoostOfEnergyStates::DEACTIVATING:
	{
		deactivationTimer += deltaTime;
		if (deactivationTimer >= deactivationDuration)
		{
			RechargeLaser();
		}
	}
		break;

	case BoostOfEnergyStates::RECHARGING:
		cooldownTimer += deltaTime;
		if (cooldownTimer >= cooldownDuration)
		{
			cooldownTimer = 0.0f;
			attackState = BoostOfEnergyStates::RECHARGED;
		}
		break;

	case BoostOfEnergyStates::RECHARGED:
		PerformAttack();
		break;
	}
}

void BoostOfEnergy::PerformAttack()
{
	aimingTimer = 0.0f;
	aimingParticle->Play();
	attackState = BoostOfEnergyStates::AIMING;
}

void BoostOfEnergy::PreShootLaser()
{
	aimingParticle->Stop();
	aimingTimer = 0.0f;
	preshootingTimer = 0.0f;
	preshootingParticle->Play();
	attackState = BoostOfEnergyStates::PRESHOOTING;
}

void BoostOfEnergy::ShootLaser()
{
	preshootingTimer = 0.0f;
	shootingTimer = 0.0f;
	mesh->GetOwner()->Enable();
	light->Enable();
	shootingParticle->Play();
	attackState = BoostOfEnergyStates::SHOOTING;
}

void BoostOfEnergy::DeactivateLaser()
{
	shootingTimer = 0.0f;
	lastDamageTime = damageFrequency;
	shootingParticle->Stop();
	light->Disable();
	mesh->GetOwner()->Disable();
	deactivatingParticle->Play();
	attackState = BoostOfEnergyStates::DEACTIVATING;
}

void BoostOfEnergy::RechargeLaser()
{
	deactivationTimer = 0.0f;
	attackState = BoostOfEnergyStates::RECHARGING;
}


bool BoostOfEnergy::IsAttackAvailable()
{
	return attackState == BoostOfEnergyStates::RECHARGED;
}

void BoostOfEnergy::SetCollisionEnter(ComponentRigidBody* other)
{
	isPlayerInDamageZone = true;
}

void BoostOfEnergy::SetCollisionExit(ComponentRigidBody* other)
{
	isPlayerInDamageZone = false;
}