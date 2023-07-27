#include "StdAfx.h"
#include "UIMissionTrigger.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(UIMissionTrigger);

UIMissionTrigger::UIMissionTrigger() : Script(), missionLevel(nullptr), lastMissionLevel(nullptr),
textBox(nullptr), maxTimeTextImageOn(5.0f)
{
	REGISTER_FIELD(missionLevel, GameObject*);
	REGISTER_FIELD(lastMissionLevel, GameObject*);
	REGISTER_FIELD(textBox, GameObject*);
	REGISTER_FIELD(maxTimeTextImageOn, float);
	
}

UIMissionTrigger::~UIMissionTrigger()
{

}

void UIMissionTrigger::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer()->GetComponent<ComponentPlayer>();
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();
	
	if (missionLevel != nullptr)
	{
		missionImageDisplacement = missionLevel->GetComponent<UIImageDisplacementControl>();
	}
	if (lastMissionLevel != nullptr)
	{
		missionImageDisplacementExit = lastMissionLevel->GetComponent<UIImageDisplacementControl>();
	}
}

void UIMissionTrigger::Update(float deltaTime)
{
	if (missionImageDisplacementExit != nullptr && !missionImageDisplacementExit->GetIsMoving() && wasInside)
	{
		missionImageDisplacement->SetMovingToEnd(true);
		missionImageDisplacement->MoveImageToEndPosition();
	}
	if (textBox != nullptr && textBox->IsEnabled())
	{
		DisableTextBox(deltaTime);
	}
}

void UIMissionTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		if (!wasInside)
		{
			if (lastMissionLevel != nullptr)
			{
				missionImageDisplacementExit->SetMovingToEnd(false);
				missionImageDisplacementExit->MoveImageToStarPosition();
			}
			else if (missionLevel != nullptr)
			{
				missionImageDisplacement->SetMovingToEnd(true);
				missionImageDisplacement->MoveImageToEndPosition();
			}

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
	}
}

void UIMissionTrigger::DisableTextBox(float time)
{
	currentTime += time;
	if (currentTime >= maxTimeTextImageOn)
	{
		textBox->Disable();
	}
	
}