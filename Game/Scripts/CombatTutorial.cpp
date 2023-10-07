#include "StdAfx.h"
#include "..\Game\Scripts\TutorialSystem.h"
#include "..\Game\Scripts\CombatTutorial.h"


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
#include "Components/ComponentTransform.h"

#include "..\Game\Scripts\HealthSystem.h"
#include "..\Game\Scripts\PlayerManagerScript.h"
#include "..\Game\Scripts\UIImageDisplacementControl.h"
#include "../Scripts/PowerUpLogicScript.h"

#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(CombatTutorial);

CombatTutorial::CombatTutorial() : Script(), combatDummy(nullptr), userControllable(false), combatTutorialUI(nullptr), 
debugPowerUp(nullptr), finalWaitTime(2.0f), tutorialActivable(false), nextStateActive(true)
{
	REGISTER_FIELD(combatDummy, GameObject*);
	REGISTER_FIELD(userControllable, bool);
	REGISTER_FIELD(combatTutorialUI, GameObject*);
	REGISTER_FIELD(debugPowerUp, GameObject*);
}

void CombatTutorial::Start()
{
	input = App->GetModule<ModuleInput>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	
	tutorialUI = combatTutorialUI->GetComponent<TutorialSystem>();
	dummyHealthSystem = combatDummy->GetComponent<HealthSystem>();

	dummyHealthSystem->SetIsImmortal(true);
}

void CombatTutorial::Update(float deltaTime)
{
	//Normal Attacks XXX - XXY
	if (tutorialActivable && userControllable && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN && !tutorialUI->GetDisplacementControl()->IsMoving())
	{
		
		tutorialUI->UnDeployUI();
		dummyHealthSystem->SetIsImmortal(true);
		
		if (tutorialUI->GetTutorialCurrentState() == int(tutorialUI->GetNumControllableState()))
		{
			dummyHealthSystem->SetIsImmortal(false);
			userControllable = false;
			nextStateActive = false;	
		}
	}
	else if (dummyHealthSystem->GetCurrentHealth() <= dummyHealthSystem->GetMaxHealth() * 0.75 
		&& dummyHealthSystem->GetCurrentHealth() > dummyHealthSystem->GetMaxHealth() * 0.50 && !nextStateActive)
		 {
		//JumpAttack
		LOG_INFO("Tutorial:JumpAttack");
			
			tutorialUI->UnDeployUI();
			dummyHealthSystem->SetIsImmortal(false);
			
			nextStateActive = true;
		 }

	else if (dummyHealthSystem->GetCurrentHealth() <= dummyHealthSystem->GetMaxHealth() * 0.50 
		&& dummyHealthSystem->GetCurrentHealth() > dummyHealthSystem->GetMaxHealth() * 0.25 && nextStateActive)
		 {
		//SpecialLightAttack
		LOG_INFO("Tutorial:SpecialLightAttack");
		
		tutorialUI->UnDeployUI();
		dummyHealthSystem->SetIsImmortal(false);
		
			nextStateActive = false;
		

		 }
	else if (dummyHealthSystem->GetCurrentHealth() <= dummyHealthSystem->GetMaxHealth() * 0.25 
		&& dummyHealthSystem->GetCurrentHealth() > 0 && !nextStateActive)
		 {
		//SpecialHeavyAttack
		LOG_INFO("Tutorial:SpecialHeavyAttack");
		
		tutorialUI->UnDeployUI();
		dummyHealthSystem->SetIsImmortal(false);
	
			//tutorialUI->NextState();
			nextStateActive = true;
		

		 }

	else if (dummyHealthSystem->GetCurrentHealth() <= 0 && nextStateActive)
	{
		//SpecialHeavyAttack
		LOG_INFO("Tutorial:END");

		tutorialUI->UnDeployUI();
		dummyHealthSystem->SetIsImmortal(false);
		nextStateActive = false;

		if (debugPowerUp != nullptr)
		{
			PowerUpLogicScript* newPowerUpLogic = debugPowerUp->GetComponent<PowerUpLogicScript>();
			ComponentTransform* ownerTransform = player->GetComponent<ComponentTransform>();

			newPowerUpLogic->ActivatePowerUp(ownerTransform->GetOwner());
		}

		userControllable = true;
		tutorialFinished = true;
	}

	if (tutorialFinished && !nextStateActive)
	{
		finalWaitTime -= deltaTime;

	}

	if (tutorialFinished && !nextStateActive && finalWaitTime <= 0)
	{
		
		tutorialUI->UnDeployUI();
		tutorialFinished = false;
		tutorialActivable = false;
		finalWaitTime = 2.0f;
	}

}

void CombatTutorial::OnCollisionEnter(ComponentRigidBody* other)
{

		if (other->GetOwner()->CompareTag("Player"))
		{
			PlayerManagerScript* playerManager = other->GetOwner()->GetComponent<PlayerManagerScript>();
			tutorialActivable = true;
			userControllable = true;
			//Launches intro
			tutorialUI->TutorialStart();
			LOG_INFO("TutorialEntered");
		}
}

void CombatTutorial::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && !tutorialFinished)
	{
		tutorialUI->TutorialEnd();
		LOG_INFO("TutorialExit");
		
	}
}

