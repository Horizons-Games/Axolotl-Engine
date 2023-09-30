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

CombatTutorial::CombatTutorial() : Script(), combatDummy(nullptr), userControllable(false)
{
	REGISTER_FIELD(combatDummy, GameObject*);
	REGISTER_FIELD(userControllable, bool);
}

void CombatTutorial::Start()
{
	input = App->GetModule<ModuleInput>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	tutorialUI = combatTutorialUI->GetComponent<TutorialSystem>();
	dummyHealthSystem = combatDummy->GetComponent<HealthSystem>();

	tutorialActivable = false;
	userControllable = false;
	dummyHealthSystem->SetIsImmortal(true);
	nextStateActive = false;

	
}

void CombatTutorial::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
	{
		tutorialUI->NextState();
		if (tutorialUI->GetTutorialCurrentState() == 1)
		{
			
			userControllable = false;
			nextStateActive = true;
		
		}
	}

	else if (dummyHealthSystem->GetCurrentHealth() <= dummyHealthSystem->GetMaxHealth() * 0.75 
		&& dummyHealthSystem->GetCurrentHealth() > dummyHealthSystem->GetMaxHealth() * 0.50)
	{
		if (nextStateActive)
		{
			tutorialUI->NextState();
			nextStateActive = false;
		}
		
	
	}

	else if (dummyHealthSystem->GetCurrentHealth() <= dummyHealthSystem->GetMaxHealth() * 0.50 
		&& dummyHealthSystem->GetCurrentHealth() > dummyHealthSystem->GetMaxHealth() * 0.25)
	{
		nextStateActive = true;

		if (nextStateActive)
		{
			tutorialUI->NextState();
			nextStateActive = false;
		}

	}

	else if (dummyHealthSystem->GetCurrentHealth() <= dummyHealthSystem->GetMaxHealth() * 0.25 
		&& dummyHealthSystem->GetCurrentHealth() >= 0)
	{
		nextStateActive = true;

		if (nextStateActive)
		{
			tutorialUI->NextState();
			nextStateActive = false;
		}

	}

	

}




 void CombatTutorial::OnCollisionEnter(ComponentRigidBody* other)
{

		if (other->GetOwner()->CompareTag("Player"))
		{
			PlayerManagerScript* playerManager = other->GetOwner()->GetComponent<PlayerManagerScript>();
			tutorialActivable = true;

			if (!playerManager->IsTeleporting() && tutorialActivable 
			&& !userControllable && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN)
			{
				tutorialUI->TutorialStart();
				dummyHealthSystem->SetIsImmortal(false);
				userControllable = true;
			}
		}

}

 void CombatTutorial::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		tutorialActivable = false;
		tutorialUI->TutorialEnd();
		//componentAnimation->SetParameter("IsActive", false);
		//// Until the trigger works 100% of the time better cross a closed door than be closed forever
		//componentRigidBody->Enable();
		//componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
	}
}

