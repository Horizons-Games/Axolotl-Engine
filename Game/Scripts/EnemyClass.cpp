#include "EnemyClass.h"
#include "Components/ComponentParticleSystem.h"
#include "GameObject/GameObject.h"

EnemyClass::EnemyClass() : stunned(false), timeStunned(0.0f)
{
	REGISTER_FIELD(particleTargetGO, GameObject*);
}

EnemyClass::~EnemyClass()
{
}

void EnemyClass::Init() 
{
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