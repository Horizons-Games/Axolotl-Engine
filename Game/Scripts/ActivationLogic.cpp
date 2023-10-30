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
componentAudio(nullptr), activeState(ActiveActions::INACTIVE), wasActivatedByPlayer(false),
wasActivatedByEnemy(false), isSmallDoor(false)
{
	REGISTER_FIELD(linkedHackZone, HackZoneScript*);
	REGISTER_FIELD(interactWithEnemies, bool);
	REGISTER_FIELD(enemiesToSpawn, GameObject*);
	REGISTER_FIELD(elevator, ElevatorCore*);
	REGISTER_FIELD(isSmallDoor, bool);
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
		enemiesWaiting.reserve(enemiesToSpawn->GetChildren().size());
	}
	//componentRigidBody->Disable();
}

void ActivationLogic::Update(float deltaTime)
{
	if (!componentRigidBody->IsEnabled() 
		&& App->GetModule<ModulePlayer>()->IsInCombat() 
		&& !wasActivatedByEnemy)
	{
		CloseDoor();
	}

	if (wasActivatedByPlayer && !App->GetModule<ModulePlayer>()->IsInCombat())
	{
		OpenDoor();
		wasActivatedByPlayer = false;
	}

	if ( interactWithEnemies
		&& App->GetModule<ModulePlayer>()->IsInBossCombat() 
		&& !elevator->GetBooked())
	{
		elevator->SetBooked(false);
	}
	if (!enemiesWaiting.empty()) 
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

		if (!App->GetModule<ModulePlayer>()->IsInCombat()) 
		{
			OpenDoor();
		}
		else 
		{
			wasActivatedByPlayer = true;
		}
		else if (other->GetOwner()->CompareTag("PlayerSpaceshipMesh"))
		{
			componentAnimation->SetParameter("IsActive", true);
			componentRigidBody->Disable();
			componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
		}
	}

	if (interactWithEnemies)
	{
		if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("PriorityTarget"))
		{
			enemiesWaiting.push_back(other->GetOwner());
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
		if (other->GetOwner()->CompareTag("Enemy") || other->GetOwner()->CompareTag("PriorityTarget"))
		{
			wasActivatedByEnemy = false;
			CloseDoor();
		}
	}
}

void ActivationLogic::NextInTheList()
{
	elevator->SetBooked(true);
	elevator->SetDisableInteractionsEnemies(enemiesWaiting[0],false, false, false);
	enemiesWaiting.erase(enemiesWaiting.begin());
	wasActivatedByEnemy = true;
	OpenDoor();
}


void ActivationLogic::OpenDoor() 
{
	componentAnimation->SetParameter("IsActive", true);
	componentRigidBody->Disable();
	if (!isSmallDoor)
	{
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
	}
	else
	{
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SPACESTATION::SMALL_DOOR);
	}
}

void ActivationLogic::CloseDoor() 
{
	componentAnimation->SetParameter("IsActive", false);
	componentRigidBody->Enable();
	if (!isSmallDoor)
	{
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
	}
	else
	{
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SPACESTATION::SMALL_DOOR_CLOSE);
	}
}