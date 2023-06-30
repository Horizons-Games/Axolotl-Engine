#include "StdAfx.h"
#include "UIMissionTrigger.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"

#include "UIGameManager.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(UIMissionTrigger);

UIMissionTrigger::UIMissionTrigger() : Script(), setUIManagerObject(nullptr), setPlayer(nullptr)
{
	REGISTER_FIELD(setUIManagerObject, GameObject*);

}

UIMissionTrigger::~UIMissionTrigger()
{

}

void UIMissionTrigger::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();

	componentRigidBody = owner->GetComponent<ComponentRigidBody>();

	if (setUIManagerObject != nullptr)
	{
		UIGameManagerClass = setUIManagerObject->GetComponent<UIGameManager>();
	}
}

void UIMissionTrigger::Update(float deltaTime)
{

}

void UIMissionTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		UIGameManagerClass->ActivateMissionImage();
	}
}

void UIMissionTrigger::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{

	}
}