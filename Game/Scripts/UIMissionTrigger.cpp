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
	
	if (missionLevel)
	{
		missionImageDisplacement = missionLevel->GetComponent<UIImageDisplacementControl>();
	}
	if (lastMissionLevel)
	{
		missionImageDisplacementExit = lastMissionLevel->GetComponent<UIImageDisplacementControl>();
	}
}

void UIMissionTrigger::Update(float deltaTime)
{
	if (missionImageDisplacement->IsImageInEndPosition())
	{
		owner->Disable();
	}

	if (missionImageDisplacementExit && !missionImageDisplacementExit->IsMoving() && wasInside 
		&& missionImageDisplacement->IsMovingToEnd())
	{
		if (lastMissionLevel)
		{
			if (missionImageDisplacementExit->IsImageInStartPosition())
			{
				missionImageDisplacement->SetMovingToEnd(true);
				missionImageDisplacement->MoveImageToEndPosition();
			}
		}
		else
		{
			missionImageDisplacement->SetMovingToEnd(true);
			missionImageDisplacement->MoveImageToEndPosition();
		}
	}
	if (textBox && textBox->IsEnabled() && hasTimer)
	{
		DisableTextBox(deltaTime);
	}

	if (!missionCondition && !App->GetModule<ModulePlayer>()->IsInCombat() &&
		wasInside && waitForNotInCombat)
	{
		if (lastMissionLevel)
		{
			missionImageDisplacementExit->SetMovingToEnd(false);
			missionImageDisplacementExit->MoveImageToStartPosition();
		}
		if (missionLevel)
		{
			if (lastMissionLevel)
			{
				if (missionImageDisplacementExit->IsImageInStartPosition())
				{
					missionImageDisplacement->SetMovingToEnd(true);
					missionImageDisplacement->MoveImageToEndPosition();
					missionCondition = false;
				}
			}
			else
			{
				missionImageDisplacement->SetMovingToEnd(true);
				missionImageDisplacement->MoveImageToEndPosition();
				missionCondition = false;
			}
		}

		if (textBox)
		{
			textBox->Enable();
		}
		
	}
	else if (waitForSwitch && !missionCondition)
	{
		if (switchManager->GetComponent<SwitchPlayerManagerScript>()->IsSwitchAvailable())
		{
			if (lastMissionLevel)
			{
				missionImageDisplacementExit->SetMovingToEnd(false);
				missionImageDisplacementExit->MoveImageToStartPosition();
			}
			if (missionLevel)
			{
				if (lastMissionLevel)
				{
					if (missionImageDisplacementExit->IsImageInStartPosition())
					{
						missionImageDisplacement->SetMovingToEnd(true);
						missionImageDisplacement->MoveImageToEndPosition();
						missionCondition = false;
					}
				}
				else
				{
					missionImageDisplacement->SetMovingToEnd(true);
					missionImageDisplacement->MoveImageToEndPosition();
					missionCondition = false;
				}
			}

			if (textBox)
			{
				textBox->Enable();
			}
		}
	}
}

void UIMissionTrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if ((other->GetOwner()->CompareTag("Player") || other->GetOwner()->CompareTag("PlayerSpaceship")) && !wasInside)
	{
		if (!waitForNotInCombat && !waitForSwitch)
		{
			if (lastMissionLevel)
			{
				missionImageDisplacementExit->SetMovingToEnd(false);
				missionImageDisplacementExit->MoveImageToStartPosition();
			}
			if (missionLevel)
			{
				missionImageDisplacement->SetMovingToEnd(true);
				if (lastMissionLevel)
				{
					if (missionImageDisplacementExit->IsImageInStartPosition())
					{
						missionImageDisplacement->MoveImageToEndPosition();
						missionCondition = false;
					}
				}
				else
				{
					missionImageDisplacement->MoveImageToEndPosition();
					missionCondition = false;
				}
			}

			if (textBox)
			{
				textBox->Enable();
			}
		}
		else if (!App->GetModule<ModulePlayer>()->IsInCombat() && waitForNotInCombat)
		{
			if (lastMissionLevel)
			{
				missionImageDisplacementExit->SetMovingToEnd(false);
				missionImageDisplacementExit->MoveImageToStartPosition();
			}
			if (missionLevel)
			{
				missionImageDisplacement->SetMovingToEnd(true);
				if (lastMissionLevel)
				{
					if (missionImageDisplacementExit->IsImageInStartPosition())
					{
						missionImageDisplacement->MoveImageToEndPosition();
						missionCondition = false;
					}
				}
				else
				{
					missionImageDisplacement->MoveImageToEndPosition();
					missionCondition = false;
				}
			}

			if (textBox)
			{
				textBox->Enable();
			}
		}
		else if (waitForSwitch)
		{
			if (switchManager->GetComponent<SwitchPlayerManagerScript>()->IsSwitchAvailable())
			{
				if (lastMissionLevel)
				{
					missionImageDisplacementExit->SetMovingToEnd(false);
					missionImageDisplacementExit->MoveImageToStartPosition();
				}
				if (missionLevel)
				{
					missionImageDisplacement->SetMovingToEnd(true);
					if (lastMissionLevel)
					{
						if (missionImageDisplacementExit->IsImageInStartPosition())
						{
							missionImageDisplacement->MoveImageToEndPosition();
							missionCondition = false;
						}
					}
					else
					{
						missionImageDisplacement->MoveImageToEndPosition();
						missionCondition = false;
					}
				}

				if (textBox)
				{
					textBox->Enable();
				}
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
	if (lastMissionLevel)
	{
		missionImageDisplacementExit->SetMovingToEnd(false);
		missionImageDisplacementExit->MoveImageToStartPosition();
	}
	if (missionLevel)
	{
		missionImageDisplacement->SetMovingToEnd(true);
		if (lastMissionLevel)
		{
			if (missionImageDisplacementExit->IsImageInStartPosition())
			{
				missionImageDisplacement->MoveImageToEndPosition();
				missionCondition = false;
			}
		}
		else
		{
			missionImageDisplacement->MoveImageToEndPosition();
			missionCondition = false;
		}
	}

	if (textBox)
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