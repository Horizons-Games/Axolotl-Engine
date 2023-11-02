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
#include "..\Game\Scripts\PlayerMoveScript.h"
#include "..\Game\Scripts\PlayerAttackScript.h"
#include "..\Game\Scripts\JumpFinisherAttack.h"
#include "..\Game\Scripts\HeavyFinisherAttack.h"
#include "..\Game\Scripts\LightFinisherAttackScript.h"
#include "..\Game\Scripts\ComboManager.h"

#include "CameraControllerScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(CombatTutorial);

CombatTutorial::CombatTutorial() : Script(), combatDummy(nullptr), userControllable(false), combatTutorialUI(nullptr),
debugPowerUp(nullptr), finalWaitTime(1.0f), finalTotalWaitTime(5.0f), tutorialActivable(false), nextStateActive(true),
door(nullptr), heavyFinisher(nullptr), normalAttacksEnded(false), tutorialStay(false)
{
	REGISTER_FIELD(combatDummy, GameObject*);
	REGISTER_FIELD(userControllable, bool);
	REGISTER_FIELD(combatTutorialUI, GameObject*);
	REGISTER_FIELD(debugPowerUp, GameObject*);
	REGISTER_FIELD(finalWaitTime, float);
	REGISTER_FIELD(finalTotalWaitTime, float);
	REGISTER_FIELD(heavyFinisher, HeavyFinisherAttack*);
	REGISTER_FIELD(door, GameObject*);


}

void CombatTutorial::Start()
{
	input = App->GetModule<ModuleInput>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	componentAnimation = door->GetComponent<ComponentAnimation>();
	playerAttack = player->GetComponent<PlayerAttackScript>();
	tutorialUI = combatTutorialUI->GetComponent<TutorialSystem>();


	if (combatDummy)
	{
		dummyHealthSystem = combatDummy->GetComponent<HealthSystem>();
		jumpAttack = player->GetComponent<JumpFinisherAttack>();
		lightFinisher = player->GetComponent<LightFinisherAttackScript>();
		componentMoveScript = player->GetComponent<PlayerMoveScript>();
		comboSystem = player->GetComponent<ComboManager>();
		dummyHealthSystem->SetIsImmortal(true);
	}

	if (door)
	{
		doorRigidbody = door->GetComponent<ComponentRigidBody>();
	}
}

void CombatTutorial::Update(float deltaTime)
{
	
	switch (tutorialUI->GetTutorialCurrentState())
	{

	case 1:
		if (tutorialActivable && userControllable && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN
			&& !tutorialUI->GetDisplacementControl()->IsMoving())
		{
			//Normal Attacks
			doorRigidbody->SetIsTrigger(false);
			componentMoveScript->SetIsParalyzed(true);
			StartTutorialState();
		}

		break;

	case 2:
		if (dummyHealthSystem->GetCurrentHealth() <= dummyHealthSystem->GetMaxHealth() * 0.75f && !nextStateActive
			&& !normalAttacksEnded && tutorialStay)
		{
			AttackTutorialState();
		}
		break;

	case 3:
		if (nextStateActive && (playerAttack->GetCurrentAttackType() == AttackType::JUMPNORMAL
			|| playerAttack->GetCurrentAttackType() == AttackType::JUMPFINISHER) && tutorialStay)
		{
			JumpAttackTutorialState();
		}
		break;

	case 4:
		if (tutorialActivable && userControllable && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN
			&& !tutorialUI->GetDisplacementControl()->IsMoving())
		{
			FullBarTutorialState();
		}

		break;

	case 5:
		if (playerAttack->GetCurrentAttackType() == AttackType::LIGHTFINISHER && nextStateActive && tutorialStay)
		{
			LightSpecialState();
		}
		break;

	case 6:
		if (playerAttack->GetCurrentAttackType() == AttackType::HEAVYFINISHER && !nextStateActive && tutorialStay)
		{
			HeavySpecialState();
		}
		break;

	case 7:
		if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN && nextStateActive && tutorialStay)
		{
			PowerUpState();
		}
		break;
	}
	
	if (tutorialFinished && !nextStateActive)
	{
		finalWaitTime -= deltaTime;
	}

	if (tutorialFinished && !nextStateActive && finalWaitTime <= 0.0f)
	{
		TutorialEndState();
	}

	//SkipTutorial
	if (tutorialActivable && input->GetKey(SDL_SCANCODE_G) == KeyState::DOWN && !tutorialUI->GetDisplacementControl()->IsMoving())
	{
		tutorialUI->TutorialSkip();
		componentAnimation->SetParameter("IsActive", true);
		doorRigidbody->Disable();
	}
}

void CombatTutorial::StartTutorialState()
{
	LOG_INFO("Tutorial:NormalAttacks");
	dummyHealthSystem->SetIsImmortal(false);
	userControllable = false;
	nextStateActive = false;
	componentMoveScript->SetIsParalyzed(false);
	tutorialUI->UnDeployUI();
}

void CombatTutorial::AttackTutorialState()
{
	//JumpAttack
	LOG_INFO("Tutorial:JumpAttack");
	dummyHealthSystem->SetIsImmortal(false);
	tutorialUI->UnDeployUI();
	dummyHealthSystem->SetMaxHealth(2000.0f);
	dummyHealthSystem->HealLife(dummyHealthSystem->GetMaxHealth());
	nextStateActive = true;
}

void CombatTutorial::JumpAttackTutorialState()
{
	//SpecialAttacks
	LOG_INFO("Tutorial:SpecialAttacks");
	normalAttacksEnded = true;
	tutorialUI->UnDeployUI();
	comboSystem->FillComboBar();
	tutorialUI->SetNumControllableState(tutorialUI->GetNumControllableState() + 3);
	userControllable = true;
	nextStateActive = false;

}

void CombatTutorial::FullBarTutorialState()
{
	//SpecialLightAttack
	LOG_INFO("Tutorial:SpecialLightAttack");
	componentMoveScript->SetIsParalyzed(false);
	comboSystem->FillComboBar();
	userControllable = false;
	nextStateActive = true;
	tutorialUI->UnDeployUI();
}

void CombatTutorial::LightSpecialState()
{

	//SpecialHeavyAttack
	LOG_INFO("Tutorial:SpecialHeavyAttack");

	tutorialUI->UnDeployUI();
	comboSystem->FillComboBar();
	nextStateActive = false;

}

void CombatTutorial::HeavySpecialState()
{
	//SpecialHeavyAttack
	tutorialUI->UnDeployUI();
	dummyHealthSystem->TakeDamage(dummyHealthSystem->GetCurrentHealth());
	if (debugPowerUp != nullptr)
	{
		PowerUpLogicScript* newPowerUpLogic = debugPowerUp->GetComponent<PowerUpLogicScript>();
		ComponentTransform* ownerTransform = player->GetComponent<ComponentTransform>();

		newPowerUpLogic->ActivatePowerUp(ownerTransform->GetOwner());
	}
	nextStateActive = true;
	LOG_INFO("Dummy:Dead");

}

void CombatTutorial::PowerUpState()
{
	//PowerUps
	tutorialUI->UnDeployUI();

	userControllable = true;
	tutorialFinished = true;

	nextStateActive = false;

	LOG_INFO("PowerUps");
}

void CombatTutorial::TutorialEndState()
{
	//tutorialUI->UnDeployUI();
	tutorialUI->TutorialEnd();
	tutorialFinished = false;
	tutorialActivable = false;
	finalWaitTime = finalTotalWaitTime;
	componentAnimation->SetParameter("IsActive", true);
	doorRigidbody->Disable();
	doorRigidbody->SetIsTrigger(true);
	LOG_INFO("Tutorial:END");

}


void CombatTutorial::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		PlayerManagerScript* playerManager = other->GetOwner()->GetComponent<PlayerManagerScript>();
		App->GetModule<ModulePlayer>()->SetInCombat(true);
		tutorialActivable = true;
		userControllable = true;
		tutorialStay = true;
		//Launches intro
		tutorialUI->TutorialStart();
		LOG_INFO("TutorialEntered");
	}
}

void CombatTutorial::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") && !tutorialFinished)
	{
		App->GetModule<ModulePlayer>()->SetInCombat(false);
		tutorialUI->TutorialEnd();
		tutorialStay = false;

		if(!tutorialActivable)
		{
			combatTutorialUI->Disable();
			LOG_INFO("TutorialDisabled");
		}
		LOG_INFO("TutorialExit");
	}
}
