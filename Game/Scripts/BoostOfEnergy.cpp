#include "StdAfx.h"
#include "BoostOfEnergy.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentLight.h"

REGISTERCLASS(BoostOfEnergy);

BoostOfEnergy::BoostOfEnergy() : Script(), preshootingDuration(0.5f), preshootingTimer(0.0f),
aimingDuration(2.0f), aimingTimer(0.0f), aimingParticle(nullptr), preshootingParticle(nullptr), 
shootingParticle(nullptr), attackDamage(30.0f), rigidBody(nullptr), target(nullptr), 
attackState(BoostOfEnergyStates::RECHARGED), shootingDuration(1.0f), shootingTimer(0.0f),
cooldownDuration(5.0f), cooldownTimer(0.0f), mesh(nullptr), light(nullptr)
{
	REGISTER_FIELD(cooldownDuration, float);
	REGISTER_FIELD(aimingDuration, float);
	REGISTER_FIELD(preshootingDuration, float);
	REGISTER_FIELD(shootingDuration, float);
	REGISTER_FIELD(aimingParticle, ComponentParticleSystem*);
	REGISTER_FIELD(preshootingParticle, ComponentParticleSystem*);
	REGISTER_FIELD(shootingParticle, ComponentParticleSystem*);
	REGISTER_FIELD(mesh, ComponentMeshRenderer*);
	REGISTER_FIELD(light, ComponentLight*);
	REGISTER_FIELD(target, ComponentTransform*);
}

void BoostOfEnergy::Start()
{
	//rigidBody = owner->GetComponent<ComponentRigidBody>();
	mesh->Disable();
	light->Disable();
	//Delete
	PerformAttack();
}

void BoostOfEnergy::Update(float deltaTime)
{
	switch(attackState)
	{
	case BoostOfEnergyStates::AIMING: //Preparing laser while aiming to the player
		aimingTimer += deltaTime;
		if (aimingTimer >= aimingDuration)
		{
			PreShootLaser();
		}
		break;
	case BoostOfEnergyStates::PRESHOOTING: //Stop aiming to the player (stay without moving) and charging laser
		preshootingTimer += deltaTime;
		if (preshootingTimer >= preshootingDuration)
		{
			ShootLaser();
		}
		break;
	case BoostOfEnergyStates::SHOOTING: //Shooting laser without moving for shootingDuration time
		shootingTimer += deltaTime;
		if (shootingTimer >= shootingDuration)
		{
			shootingTimer = 0.0f;
			shootingParticle->Stop();
			mesh->Disable();
			light->Disable();
			//deactivate rigidBody
			attackState = BoostOfEnergyStates::RECHARGING;
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
	mesh->Enable();
	light->Enable();
	shootingParticle->Play();
	attackState = BoostOfEnergyStates::SHOOTING;
}


bool BoostOfEnergy::IsAttackAvailable()
{
	return attackState == BoostOfEnergyStates::RECHARGED;
}

