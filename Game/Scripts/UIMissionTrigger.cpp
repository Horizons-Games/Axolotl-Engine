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
textBox(nullptr), maxTimeTextImageOn(5.0f), hasTimer(false), waitForNotInCombat(false)
{
	REGISTER_FIELD(missionLevel, GameObject*);
	REGISTER_FIELD(lastMissionLevel, GameObject*);
	REGISTER_FIELD(textBox, GameObject*);
	REGISTER_FIELD(hasTimer, bool);
	REGISTER_FIELD(waitForNotInCombat, bool);
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
	if (missionImageDisplacementExit != nullptr && !missionImageDisplacementExit->IsMoving() && wasInside 
		&& missionImageDisplacement->IsMovingToEnd())
	{
		missionImageDisplacement->SetMovingToEnd(true);
		missionImageDisplacement->MoveImageToEndPosition();
	}
	if (textBox != nullptr && textBox->IsEnabled() && hasTimer)
	{
		DisableTextBox(deltaTime);
	}

	if (!notInCombat && !App->GetModule<ModulePlayer>()->IsInCombat() &&
		wasInside)
	{
		if (lastMissionLevel != nullptr)
		{
			missionImageDisplacementExit->SetMovingToEnd(false);
			missionImageDisplacementExit->MoveImageToStartPosition();
		}
		if (missionLevel != nullptr)
		{
			missionImageDisplacement->SetMovingToEnd(true);
			missionImageDisplacement->MoveImageToEndPosition();
		}

		if (textBox != nullptr)
		{
			textBox->Enable();
		}
		notInCombat = true;
	}
}

void UIMissionTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && !wasInside)
	{
		if (!wasInside && !waitForNotInCombat)
		{
			if (lastMissionLevel != nullptr)
			{
				missionImageDisplacementExit->SetMovingToEnd(false);
				missionImageDisplacementExit->MoveImageToStartPosition();
			}
			if (missionLevel != nullptr)
			{
				missionImageDisplacement->SetMovingToEnd(true);
				missionImageDisplacement->MoveImageToEndPosition();
			}

			if (textBox != nullptr)
			{
				textBox->Enable();
			}
		}
		else if (!App->GetModule<ModulePlayer>()->IsInCombat())
		{
			if (lastMissionLevel != nullptr)
			{
				missionImageDisplacementExit->SetMovingToEnd(false);
				missionImageDisplacementExit->MoveImageToStartPosition();
			}
			if (missionLevel != nullptr)
			{
				missionImageDisplacement->SetMovingToEnd(true);
				missionImageDisplacement->MoveImageToEndPosition();
			}

			if (textBox != nullptr)
			{
				textBox->Enable();
			}
			notInCombat = false;
		}

		wasInside = true;
	}
}

void UIMissionTrigger::OnCollisionExit(ComponentRigidBody* other)
{
}

void UIMissionTrigger::DisableTextBox(float time)
{
	currentTime += time;
	if (currentTime >= maxTimeTextImageOn)
	{
		textBox->Disable();
	}
	
}