#include "StdAfx.h"
#include "ForceTrigger.h"

#include "Application.h"
#include "ModulePlayer.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"
#include "PlayerForceUseScript.h"

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
			App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<PlayerForceUseScript>()->SetInsideForceZone(true);
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
			App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<PlayerForceUseScript>()->SetInsideForceZone(false);
		}
	}
}
