#include "StdAfx.h"
#include "TutorialSystem.h"
#include "SwitchTutorial.h"
#include "SwitchPlayerManagerScript.h"


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


REGISTERCLASS(SwitchTutorial);

SwitchTutorial::SwitchTutorial() : Script(), allura(nullptr), combatTutorialUI(nullptr),
finalWaitTime(5.0f), finalTotalWaitTime(5.0f),tutorialActivable(false), manager(nullptr)
{
	REGISTER_FIELD(allura, GameObject*);
	REGISTER_FIELD(manager, GameObject*);
	REGISTER_FIELD(combatTutorialUI, GameObject*);
	REGISTER_FIELD(finalWaitTime, float);
	REGISTER_FIELD(finalTotalWaitTime, float);
}

void SwitchTutorial::Start()
{
	input = App->GetModule<ModuleInput>();
	player = App->GetModule<ModulePlayer>()->GetPlayer();
	playerManager = player->GetComponent<PlayerManagerScript>();
	
	tutorialUI = combatTutorialUI->GetComponent<TutorialSystem>();
}

void SwitchTutorial::Update(float deltaTime)
{
	if (tutorialActivable && input->GetKey(SDL_SCANCODE_F) == KeyState::DOWN && !tutorialUI->GetDisplacementControl()->IsMoving())
	{	
		tutorialUI->UnDeployUI();
		
		if (tutorialUI->GetTutorialCurrentState() == tutorialUI->GetTutorialSlideSize())
		{
			manager->GetComponent<SwitchPlayerManagerScript>()->SetIsSwitchAvailable(true);
			tutorialFinished = true;
			playerManager->PausePlayer(false);
		}
	}

	if (tutorialFinished)
	{
		finalWaitTime -= deltaTime;

	}

	if (tutorialFinished && finalWaitTime <= 0.0f)
	{
		
		tutorialUI->UnDeployUI();
		tutorialFinished = false;
		tutorialActivable = false;
		finalWaitTime = finalTotalWaitTime;
		LOG_INFO("Tutorial:END");
		owner->Disable();
	}

}

void SwitchTutorial::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		playerManager->PausePlayer(true);
		tutorialActivable = true;
		//Launches intro
		tutorialUI->TutorialStart();
		LOG_INFO("TutorialEntered");
	}
}

