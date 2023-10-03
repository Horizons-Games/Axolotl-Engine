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

#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(TutorialSystem);

TutorialSystem::TutorialSystem() :
	Script(), userControllable(false), tutorialCurrentState(0), tutorialTotalStates(0), stateWaitTime(0.0f), 
	totalStateWaitTime(0.0f), dummy(nullptr), numNotControllableStates(0.0f)
{

	REGISTER_FIELD(userControllable, bool);
	REGISTER_FIELD(totalStateWaitTime, float);
	REGISTER_FIELD(stateWaitTime, float);

	REGISTER_FIELD(numNotControllableStates, float);

	/*REGISTER_FIELD(tutorialCurrentState, float);
	REGISTER_FIELD(tutorialTotalStates, float);*/

	REGISTER_FIELD(tutorialUI, std::vector<GameObject*>);
	REGISTER_FIELD(dummy, GameObject*);
}



void TutorialSystem::Start()
{
	
	currentTutorialUI = tutorialUI.front();
	displacementControl = currentTutorialUI->GetComponent<UIImageDisplacementControl>();
	isWaiting = false;
	stateWaitTime = totalStateWaitTime;
	dummyHealthSystem = dummy->GetComponent<HealthSystem>();
}

void TutorialSystem::Update(float deltaTime)
{
	if (isWaiting)
	{
		stateWaitTime-= deltaTime;
	}

	if (stateWaitTime <= 0)
	{
		isWaiting = false;
		stateWaitTime = totalStateWaitTime;
		DeployUI();
		
		
	}

}

void TutorialSystem::TutorialStart()
{
	
	tutorialCurrentState = 0;
	tutorialTotalStates =  tutorialUI.size();
	currentTutorialUI->Enable();
	displacementControl->MoveImageToEndPosition();
	

}

void TutorialSystem::NextState()
{
	if (tutorialCurrentState < tutorialTotalStates)
	{
		UnDeployUI();
		
	}

	else if (tutorialCurrentState >= tutorialTotalStates)
	{
		TutorialEnd();

	}
	

}

void TutorialSystem::DeployUI()
{
	currentTutorialUI->Enable();
	
	displacementControl->MoveImageToEndPosition();

	if (tutorialCurrentState == int(numNotControllableStates))
	{
		dummyHealthSystem->SetIsImmortal(false);

	}
	
	
	//NextTutorialUI = tutorialUI[tutorialCurrentState + 1];

	
	
}

//void TutorialSystem::DeployUI()
//{
//
//	//currentTutorialUI->Disable();
//	displacementControl->MoveImageToEndPosition();
//
//	NextTutorialUI = tutorialUI[tutorialCurrentState + 1];
//
//	//displacementControl->MoveImageToStarPosition();
//	////NextTutorialUI->Enable();
//	//tutorialCurrentState++;
//	//currentTutorialUI = tutorialUI[tutorialCurrentState];
//	//displacementControl = currentTutorialUI->GetComponent<UIImageDisplacementControl>();

void TutorialSystem::UnDeployUI()
{
	isWaiting = true;
	

	displacementControl->MoveImageToStarPosition();

	//currentTutorialUI->Disable();
	//NextTutorialUI->Enable();
	tutorialCurrentState++;
	currentTutorialUI = tutorialUI[tutorialCurrentState];
	displacementControl = currentTutorialUI->GetComponent<UIImageDisplacementControl>();
	tutorialUI[tutorialCurrentState - 1]->Disable();
	dummyHealthSystem->SetIsImmortal(true);
}

void TutorialSystem::TutorialEnd()
{
	//displacementControl->MoveImageToStarPosition();
	currentTutorialUI->Disable();
	tutorialCurrentState = 0;
	currentTutorialUI = tutorialUI.front();
	
}

int TutorialSystem::GetTutorialCurrentState() const
{

	return tutorialCurrentState;

}

float TutorialSystem::GetNumControllableState() const
{

	return numNotControllableStates;

}

int TutorialSystem::GetTutorialSlideSize() const
{
	return tutorialTotalStates;
}

void TutorialSystem::SetTutorialSlideSize(int tutorialTotalStates)
{
	this->tutorialTotalStates = tutorialTotalStates;
}
