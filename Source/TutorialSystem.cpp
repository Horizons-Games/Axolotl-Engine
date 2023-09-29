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

#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(TutorialSystem);

TutorialSystem::TutorialSystem() :
	Script(), userControllable(false), tutorialCurrentState(0), tutorialTotalStates(0)
{

	REGISTER_FIELD(userControllable, bool);
	
	REGISTER_FIELD(tutorialCurrentState, int);
	REGISTER_FIELD(tutorialTotalStates, int);

	REGISTER_FIELD(tutorialUI, std::vector<GameObject*>);
}



void TutorialSystem::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	TutorialStart();
	
}

void TutorialSystem::Update(float deltaTime)
{
	

}

void TutorialSystem::TutorialStart()
{
	currentTutorialUI = tutorialUI.front();
	tutorialCurrentState = 0;
	tutorialTotalStates = tutorialUI.size();
	currentTutorialUI->Enable();

}

void TutorialSystem::NextState()
{
	if (tutorialCurrentState < tutorialTotalStates)
	{
		DeployUI();
		
	}

	else if (tutorialCurrentState >= tutorialTotalStates)
	{
		TutorialEnd();

	}
	

}

void TutorialSystem::DeployUI()
{
	
	currentTutorialUI->Disable();
	NextTutorialUI = tutorialUI[tutorialCurrentState + 1];
	NextTutorialUI->Enable();
	tutorialCurrentState++;
	currentTutorialUI = tutorialUI[tutorialCurrentState];
	
	
}

void TutorialSystem::TutorialEnd()
{
	currentTutorialUI->Disable();
	tutorialCurrentState = 0;
	currentTutorialUI = tutorialUI.front();
	
}

int TutorialSystem::GetTutorialIndex() const
{

	return tutorialCurrentState;

}

//void TutorialSystem::OnCollisionEnter(ComponentRigidBody* other)
//{
//
//		if (other->GetOwner()->CompareTag("Player"))
//		{
//			PlayerManagerScript* playerManager = other->GetOwner()->GetComponent<PlayerManagerScript>();
//			if (!playerManager->IsTeleporting())
//			{
//				componentAnimation->SetParameter("IsActive", true);
//				componentRigidBody->Disable();
//				componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
//			}
//		}
//	
//}
//
//void TutorialSystem::OnCollisionExit(ComponentRigidBody* other)
//{
//	if (other->GetOwner()->CompareTag("Player"))
//	{
//		//componentAnimation->SetParameter("IsActive", false);
//		//// Until the trigger works 100% of the time better cross a closed door than be closed forever
//		//componentRigidBody->Enable();
//		//componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
//	}
//}