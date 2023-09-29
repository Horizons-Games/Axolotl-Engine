#include "EnemyClass.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentParticleSystem.h"
#include "GameObject/GameObject.h"

#include "../Scripts/HealthSystem.h"

EnemyClass::EnemyClass() : stunned(false), timeStunned(0.0f), isSpawnedEnemy(false), enemyType(EnemyTypes::NONE),
	needsToBeReset(false)
{
	REGISTER_FIELD(particleTargetGO, GameObject*);
	REGISTER_FIELD(isSpawnedEnemy, bool);
}

EnemyClass::~EnemyClass()
{
}

void EnemyClass::Init() 
{
	GetOwner()->GetComponent<HealthSystem>()->SetDeathCallback(std::bind(&EnemyClass::SetReadyToDie, this));
	particleTarget = particleTargetGO->GetComponent<ComponentParticleSystem>();
}

void EnemyClass::VisualTarget(bool activate)
{
	if (activate)
	{
		particleTarget->Play();
	}
	else 
	{
		particleTarget->Stop();
	}
}

void EnemyClass::SetStunnedTime(float newTime)
{
	stunned = true;
	timeStunned = newTime;
}

float EnemyClass::GetStunnedTime() const
{
	return timeStunned;
}

bool EnemyClass::IsSpawnedEnemy() const
{
	return isSpawnedEnemy;
}

bool EnemyClass::NeedsToBeReset() const
{
	return needsToBeReset;
}

void EnemyClass::ActivateNeedsToBeReset()
{
	needsToBeReset = true;
}

void EnemyClass::DeactivateNeedsToBeReset()
{
	needsToBeReset = false;
}

EnemyTypes EnemyClass::GetEnemyType() const
{
	return enemyType;
}

void EnemyClass::ParalyzeEnemy(bool paralyzed) const
{
	return;
}
