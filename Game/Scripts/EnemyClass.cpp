#include "EnemyClass.h"
#include "Components/ComponentParticleSystem.h"
#include "GameObject/GameObject.h"

EnemyClass::EnemyClass() : stunned(false), timeStunned(0.0f)
{
	REGISTER_FIELD(particleGO, GameObject*);
}

EnemyClass::~EnemyClass()
{
}

void EnemyClass::Init() 
{
	particleTarget = particleGO->GetComponent<ComponentParticleSystem>();
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