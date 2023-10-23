#include "StdAfx.h"
#include "UIMissionTrigger.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Scene/Scene.h"

#include "SwitchPlayerManagerScript.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(UIMissionTrigger);

UIMissionTrigger::UIMissionTrigger() : Script(), missionLevel(nullptr), lastMissionLevel(nullptr),
textBox(nullptr), maxTimeTextImageOn(5.0f), hasTimer(false), waitForNotInCombat(false),
waitForSwitch(false), switchManager(nullptr)
{
	REGISTER_FIELD(missionLevel, GameObject*);
	REGISTER_FIELD(lastMissionLevel, GameObject*);
	REGISTER_FIELD(textBox, GameObject*);
	REGISTER_FIELD(hasTimer, bool);
	REGISTER_FIELD(waitForNotInCombat, bool);
	REGISTER_FIELD(waitForSwitch, bool);
	REGISTER_FIELD(switchManager, GameObject*);
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

	if (!missionCondition && !App->GetModule<ModulePlayer>()->IsInCombat() &&
		wasInside && waitForNotInCombat)
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
		missionCondition = true;
	}
	else if (waitForSwitch && !missionCondition)
	{
		if (switchManager->GetComponent<SwitchPlayerManagerScript>()->IsSwitchAvailable())
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
			missionCondition = false;
		}
	}
}

void UIMissionTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && !wasInside)
	{
		if (!waitForNotInCombat && !waitForSwitch)
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
		else if (!App->GetModule<ModulePlayer>()->IsInCombat() && waitForNotInCombat)
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
			missionCondition = false;
		}
		else if (waitForSwitch)
		{
			if (switchManager->GetComponent<SwitchPlayerManagerScript>()->IsSwitchAvailable())
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
				missionCondition = false;
			}
		}

		wasInside = true;
	}
}

void UIMissionTrigger::OnCollisionExit(ComponentRigidBody* other)
{
}

void UIMissionTrigger::ActivateTextBoxManually()
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
	wasInside = true;
}

void UIMissionTrigger::DisableTextBox(float time)
{
	currentTime += time;
	if (currentTime >= maxTimeTextImageOn)
	{
		textBox->Disable();
	}
	
}