#include "StdAfx.h"
#include "UIMissionTrigger.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

REGISTERCLASS(UIMissionTrigger);

UIMissionTrigger::UIMissionTrigger() : Script(), textBox(nullptr)
{
	REGISTER_FIELD(textBox, GameObject*);
}

UIMissionTrigger::~UIMissionTrigger()
{

}

void UIMissionTrigger::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();
}

void UIMissionTrigger::Update(float deltaTime)
{

}

void UIMissionTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		if (!wasInside)
		{
			if (textBox != nullptr)
			{
				textBox->Enable();
			}
			wasInside = true;
		}
	}
}

void UIMissionTrigger::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		if (wasInside)
		{
			if (textBox != nullptr)
			{
				textBox->Disable();
			}
			wasInside = true;
		}
	}
}