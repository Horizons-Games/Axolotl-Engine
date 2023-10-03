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

#include "..\Game\Scripts\HealthSystem.h"
#include "..\Game\Scripts\PlayerManagerScript.h"
#include "..\Game\Scripts\PlayerManagerScript.h"

#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(CombatTutorial);

CombatTutorial::CombatTutorial() : Script(), combatDummy(nullptr), userControllable(false), combatTutorialUI(nullptr)
{
	REGISTER_FIELD(combatDummy, GameObject*);
	REGISTER_FIELD(userControllable, bool);
	REGISTER_FIELD(combatTutorialUI, GameObject*);
}

void CombatTutorial::Start()
{
	input = App->GetModule<ModuleInput>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	
	//componentAudio = owner->GetComponent<ComponentAudioSource>();
	//componentAnimation = owner->GetComponent<ComponentAnimation>();
	tutorialUI = combatTutorialUI->GetComponent<TutorialSystem>();
	dummyHealthSystem = combatDummy->GetComponent<HealthSystem>();

	tutorialActivable = false;
	userControllable = false;
	dummyHealthSystem->SetIsImmortal(true);
	nextStateActive = true;
	userControllable = true;

	
}

void CombatTutorial::Update(float deltaTime)
{
	//Normal Attacks XXX - XXY
	if (tutorialActivable && userControllable && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
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
		tutorialFinished = true;


	}


	

}




 void CombatTutorial::OnCollisionEnter(ComponentRigidBody* other)
{

		if (other->GetOwner()->CompareTag("Player"))
		{
			PlayerManagerScript* playerManager = other->GetOwner()->GetComponent<PlayerManagerScript>();
			tutorialActivable = true;
			//Launches intro
			tutorialUI->TutorialStart();
			LOG_INFO("TutorialEntered");
		}

}

 void CombatTutorial::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && tutorialFinished)
	{
		tutorialActivable = false;
		tutorialUI->TutorialEnd();
		LOG_INFO("TutorialExit");
		
	}
}

