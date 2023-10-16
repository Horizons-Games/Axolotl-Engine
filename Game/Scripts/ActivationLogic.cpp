#include "StdAfx.h"
#include "ActivationLogic.h"

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

#include "../Scripts/CameraControllerScript.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/HackZoneScript.h"
#include "../Scripts/ElevatorCore.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(ActivationLogic);

ActivationLogic::ActivationLogic() : Script(),
componentAudio(nullptr), activeState(ActiveActions::INACTIVE), wasActivatedByPlayer(false)
{
	REGISTER_FIELD(linkedHackZone, HackZoneScript*);
	REGISTER_FIELD(interactWithEnemies, bool);
	REGISTER_FIELD(enemiesToSpawn, GameObject*);
	REGISTER_FIELD(elevator, ElevatorCore*);
}

ActivationLogic::~ActivationLogic()
{
}

void ActivationLogic::Start()
{
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	GameObject::GameObjectView children = owner->GetChildren();
	auto childWithRigid = std::find_if(std::begin(children),
									   std::end(children),
									   [](const GameObject* child)
									   {
										   return child->HasComponent<ComponentRigidBody>();
									   });
	componentRigidBody = (*childWithRigid)->GetComponent<ComponentRigidBody>();

	if(interactWithEnemies)
	{
		enemiesWating.reserve(enemiesToSpawn->GetChildren().size());
	}
	//componentRigidBody->Disable();
}

void ActivationLogic::Update(float deltaTime)
{
	if (!componentRigidBody->IsEnabled() 
		&& App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<CameraControllerScript>()->IsInCombat())
	{
		CloseDoor();
	}

	if (wasActivatedByPlayer && !App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<CameraControllerScript>()->IsInCombat())
	{
		OpenDoor();
		wasActivatedByPlayer = false;
	}

	if ( interactWithEnemies
		&& !App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<CameraControllerScript>()->IsInCombat() 
		&& !elevator->GetBooked())
	{
		elevator->SetBooked(false);
	}
	if (!enemiesWating.empty()) 
	{
		if (elevator->GetElevatorPos(PositionState::DOWN))
		{
			elevator->ActiveAuto();
		}
		else if (elevator->GetElevatorPos(PositionState::UP) && !elevator->GetBooked())
		{
			NextInTheList();
		}
	}
}

void ActivationLogic::OnCollisionEnter(ComponentRigidBody* other)
{
	if (linkedHackZone && !linkedHackZone->IsCompleted())
	{
		return;
	}

	if (other->GetOwner()->CompareTag("Player"))
	{
		PlayerManagerScript* playerManager = other->GetOwner()->GetComponent<PlayerManagerScript>();
		if (playerManager->IsTeleporting())
		{
			return;
		}

		if (!App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<CameraControllerScript>()->IsInCombat()) 
		{
			OpenDoor();
		}
		else 
		{
			wasActivatedByPlayer = true;
		}
	}

	if (interactWithEnemies)
	{
		if (other->GetOwner()->CompareTag("Enemy"))
		{
			enemiesWating.push_back(other->GetOwner());
			elevator->SetDisableInteractionsEnemies(other->GetOwner(), true, false, true);
		}
	}
}

void ActivationLogic::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		wasActivatedByPlayer = false;
		CloseDoor();
	}

	if (interactWithEnemies)
	{
		if (other->GetOwner()->CompareTag("Enemy"))
		{
			CloseDoor();
		}
	}
}

void ActivationLogic::NextInTheList()
{
	elevator->SetBooked(true);
	elevator->SetDisableInteractionsEnemies(enemiesWating[0],false, false, false);
	enemiesWating.erase(enemiesWating.begin());
	OpenDoor();
}


void ActivationLogic::OpenDoor() 
{
	componentAnimation->SetParameter("IsActive", true);
	componentRigidBody->Disable();
	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
}

void ActivationLogic::CloseDoor() 
{
	componentAnimation->SetParameter("IsActive", false);
	componentRigidBody->Enable();
	componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
}