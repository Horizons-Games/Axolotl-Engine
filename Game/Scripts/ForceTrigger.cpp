#include "StdAfx.h"
#include "ForceTrigger.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"

REGISTERCLASS(ForceTrigger);

ForceTrigger::ForceTrigger() : Script()
{
}

ForceTrigger::~ForceTrigger()
{
}

void ForceTrigger::Start()
{
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();
}

void ForceTrigger::Update(float deltaTime)
{
}

void ForceTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		if (owner->GetChildren()[0]->GetChildren()[0]->HasComponent<ComponentParticleSystem>())
		{
			owner->GetChildren()[0]->GetChildren()[0]->GetComponent<ComponentParticleSystem>()->Play();
		}
	}
}

void ForceTrigger::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		if (owner->GetChildren()[0]->GetChildren()[0]->HasComponent<ComponentParticleSystem>())
		{
			owner->GetChildren()[0]->GetChildren()[0]->GetComponent<ComponentParticleSystem>()->Stop();
		}
	}
}
