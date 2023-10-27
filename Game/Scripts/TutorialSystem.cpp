#include "StdAfx.h"
#include "TutorialSystem.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"

#include "..\Game\Scripts\PlayerManagerScript.h"
#include "..\Game\Scripts\PlayerManagerScript.h"
#include "..\Game\Scripts\UIImageDisplacementControl.h"
#include "..\Game\Scripts\HealthSystem.h"
#include "..\Game\Scripts\PlayerMoveScript.h"
#include "Components/UI/ComponentTransform2D.h"


#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(TutorialSystem);

TutorialSystem::TutorialSystem() :
	Script(), tutorialCurrentState(0), tutorialTotalStates(0), stateWaitTime(0.0f), 
	totalStateWaitTime(0.0f), dummy(nullptr), numNotControllableStates(0.0f), initialPos(-46.0f, -208.665f, 0.0f), isWaiting(false),
	stayPos(-46.0f, 208.665f, 0.0f)
{
	REGISTER_FIELD(totalStateWaitTime, float);
	REGISTER_FIELD(stateWaitTime, float); 
	REGISTER_FIELD(tutorialTotalStates, float);

	REGISTER_FIELD(numNotControllableStates, float);

	REGISTER_FIELD(tutorialUI, std::vector<GameObject*>);
	REGISTER_FIELD(dummy, GameObject*);
}



void TutorialSystem::Start()
{
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	currentTutorialUI = tutorialUI.front();
	displacementControl = currentTutorialUI->GetComponent<UIImageDisplacementControl>();
	transform2D = currentTutorialUI->GetComponent<ComponentTransform2D>();
	tutorialTotalStates = static_cast<int>(tutorialUI.size()) - 1;
	stateWaitTime = totalStateWaitTime;

	if (dummy)
	{
		dummyHealthSystem = dummy->GetComponent<HealthSystem>();
		componentMoveScript = player->GetComponent<PlayerMoveScript>();
	}
}

void TutorialSystem::Update(float deltaTime)
{
	if (isWaiting)
	{
		stateWaitTime-= deltaTime;
		if (stateWaitTime <= 0)
	    {
			isWaiting = false;
			stateWaitTime = totalStateWaitTime;
			DeployUI();
	    }
	}
}

void TutorialSystem::TutorialStart()
{
	currentTutorialUI->Enable();
	displacementControl->SetMovingToEnd(true);
	displacementControl->MoveImageToEndPosition();
}

void TutorialSystem::DeployUI()
{
	currentTutorialUI->Enable();
	ComponentTransform2D* transform2D = currentTutorialUI->GetComponent<ComponentTransform2D>();
	transform2D->SetPosition(stayPos);
	transform2D->CalculateMatrices();

	if (tutorialCurrentState == int(numNotControllableStates) && dummy)
	{
		dummyHealthSystem->SetIsImmortal(false);
	}	
}

void TutorialSystem::UnDeployUI()
{
	if (tutorialCurrentState < tutorialTotalStates)
	{
		totalStateWaitTime = 0.05f;	
		stateWaitTime = 0.05f;
		isWaiting = true;
		currentTutorialUI->Disable();

		tutorialCurrentState++;
		currentTutorialUI = tutorialUI[tutorialCurrentState];
		displacementControl = currentTutorialUI->GetComponent<UIImageDisplacementControl>();
		if (dummy)
		{
			dummyHealthSystem->SetIsImmortal(true);
		}
	}
	else
	{
		TutorialEnd();
	}
	
}

void TutorialSystem::TutorialEnd()
{
	totalStateWaitTime = 2.0f;
	stateWaitTime = 2.0f;
	displacementControl->SetMovingToEnd(false);
	displacementControl->MoveImageToStartPosition();
	componentMoveScript->SetIsParalyzed(false);
	LOG_INFO("TutorialExit");
}

void TutorialSystem::TutorialSkip()
{
	totalStateWaitTime = 2.0f;
	stateWaitTime = 2.0f;
	displacementControl->SetMovingToEnd(false);
	displacementControl->MoveImageToStartPosition();
	tutorialCurrentState = 0;
	currentTutorialUI = tutorialUI.front();
	displacementControl = currentTutorialUI->GetComponent<UIImageDisplacementControl>();
	componentMoveScript->SetIsParalyzed(false);
	owner->Disable();
	LOG_INFO("TutorialSkipped");
}

int TutorialSystem::GetTutorialCurrentState() const
{
	return tutorialCurrentState;
}

float TutorialSystem::GetNumControllableState() const
{
	return numNotControllableStates;
}

void TutorialSystem::SetNumControllableState(int controllableState) 
{
	numNotControllableStates = controllableState;
}

int TutorialSystem::GetTutorialSlideSize() const
{
	return tutorialTotalStates;
}

void TutorialSystem::SetTutorialSlideSize(int tutorialTotalStates)
{
	this->tutorialTotalStates = tutorialTotalStates;
}


UIImageDisplacementControl* TutorialSystem::GetDisplacementControl()
{
	return displacementControl;
}