#include "StdAfx.h"
#include "ElevatorCore.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "PlayerManagerScript.h"
#include "PlayerMoveScript.h"
#include "PlayerJumpScript.h"
#include "PlayerAttackScript.h"
#include "HealthSystem.h"


#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(ElevatorCore);

ElevatorCore::ElevatorCore() : Script(),
componentAudio(nullptr), activeState(ActiveActions::INACTIVE), positionState(PositionState::UP)
{
	REGISTER_FIELD(elevator, GameObject*);
	REGISTER_FIELD(finalPos, float);
	REGISTER_FIELD(coolDown, float);
	REGISTER_FIELD(speed, float);
	REGISTER_FIELD(miniBossHealth, HealthSystem*);
}

ElevatorCore::~ElevatorCore()
{
}

void ElevatorCore::Start()
{
	LOG_DEBUG("Name of object {}", owner->GetName());
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	GameObject::GameObjectView children = owner->GetChildren();
	auto childWithRigid = std::find_if(std::begin(children),
		std::end(children),
		[](const GameObject* child)
		{
			return child->HasComponent<ComponentRigidBody>();
		});
	// not just assert, since it would crash on the next line
	if (childWithRigid == std::end(children))
	{
		LOG_ERROR("Expected one of {}'s children to have a ComponentRigidBody, but none was found", GetOwner());
		throw ComponentNotFoundException("ComponentRigidBody not found in children");
	}
	componentRigidBody = (*childWithRigid)->GetComponent<ComponentRigidBody>();
	transform = elevator->GetComponentInternal<ComponentTransform>();
	triggerEntrance = owner->GetComponent<ComponentRigidBody>();
	finalUpPos = 0.0f;
	currentPlayer = App->GetModule<ModulePlayer>()->GetPlayer();
	playerTransform = currentPlayer->GetComponent<ComponentTransform>();
	currentTime = 0.0f;
}

void ElevatorCore::Update(float deltaTime)
{
	currentPlayer = App->GetModule<ModulePlayer>()->GetPlayer();
	playerTransform = currentPlayer->GetComponent<ComponentTransform>();
	float3 playerPos = playerTransform->GetGlobalPosition();

	if (activeState == ActiveActions::ACTIVE_PLAYER) 
	{
		if (positionState == PositionState::UP)
		{
			MoveDownElevator(true, deltaTime);
		}
		else 
		{
			MoveUpElevator(true, deltaTime);
		}
	}

	else if (activeState == ActiveActions::ACTIVE_AUTO)
	{
		if (positionState == PositionState::UP)
		{
			MoveDownElevator(false, deltaTime);
		}
		else
		{
			MoveUpElevator(false, deltaTime);
		}
	}

	else 
	{
		if (playerPos.y < finalPos)
		{
			if ((!miniBossHealth->EntityIsAlive() && positionState == PositionState::UP) ||
				(miniBossHealth->EntityIsAlive() && positionState == PositionState::DOWN))
			{
				activeState = ActiveActions::ACTIVE_AUTO;
			}
			
		}
		
		if (currentTime >= 0.0f)
		{
			currentTime -= deltaTime;
		}

	}
}

void ElevatorCore::MoveUpElevator(bool isPlayerInside, float deltaTime)
{
	float3 pos = transform->GetGlobalPosition();
	btVector3 triggerOrigin = triggerEntrance->GetRigidBodyOrigin();

	pos.y += deltaTime * speed;
	float newYTrigger = triggerOrigin.getY() + deltaTime * speed;
	triggerOrigin.setY(newYTrigger);

	transform->SetGlobalPosition(pos);
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();

	triggerEntrance->SetRigidBodyOrigin(triggerOrigin);
	elevator->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();

	if (pos.y >= finalUpPos)
	{
		positionState = PositionState::UP;
		activeState = ActiveActions::INACTIVE;
		currentTime = coolDown;
		if (isPlayerInside)
		{
			EnableAllInteractions();
		}
	}

	if (isPlayerInside)
	{
		float3 playerPos = playerTransform->GetGlobalPosition();
		playerPos.y += deltaTime * speed;

		playerTransform->SetGlobalPosition(playerPos);
		playerTransform->RecalculateLocalMatrix();
		playerTransform->UpdateTransformMatrices();

		currentPlayer->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
	}

}

void ElevatorCore::MoveDownElevator(bool isPlayerInside, float deltaTime)
{
	float3 pos = transform->GetGlobalPosition();
	float3 playerPos = playerTransform->GetGlobalPosition();
	btVector3 triggerOrigin = triggerEntrance->GetRigidBodyOrigin();

	pos.y -= deltaTime * speed;
	float newYTrigger = triggerOrigin.getY() - deltaTime * speed;
	triggerOrigin.setY(newYTrigger);

	transform->SetGlobalPosition(pos);
	transform->RecalculateLocalMatrix();
	transform->UpdateTransformMatrices();

	triggerEntrance->SetRigidBodyOrigin(triggerOrigin);
	elevator->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();

	if (pos.y <= finalPos)
	{
		positionState = PositionState::DOWN;
		activeState = ActiveActions::INACTIVE;
		currentTime = coolDown;
		if (isPlayerInside)
		{
			EnableAllInteractions();
		}
	}

	if (isPlayerInside)
	{
		float3 playerPos = playerTransform->GetGlobalPosition();
		playerPos.y -= deltaTime * speed;

		playerTransform->SetGlobalPosition(playerPos);
		playerTransform->RecalculateLocalMatrix();
		playerTransform->UpdateTransformMatrices();

		currentPlayer->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
	}
}


void ElevatorCore::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter of ElevatorCore", other->GetOwner());
	if (!App->GetModule<ModuleScene>()->GetLoadedScene()->GetCombatMode())
	{
		if (other->GetOwner()->CompareTag("Player"))
		{

			PlayerActions currentAction = currentPlayer->GetComponent<PlayerManagerScript>()->GetPlayerState();
			bool isJumping = currentAction == PlayerActions::JUMPING ||
				currentAction == PlayerActions::DOUBLEJUMPING ||
				currentAction == PlayerActions::FALLING;

			if (!isJumping && currentTime <= 0.0f)
			{
				//componentAnimation->SetParameter("IsActive", true);
				componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
				activeState = ActiveActions::ACTIVE_PLAYER;

				DisableAllInteractions();

			}
			
		}
	}
}

void ElevatorCore::OnCollisionExit(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionExit of ElevatorCore", other->GetOwner());
	if (!App->GetModule<ModuleScene>()->GetLoadedScene()->GetCombatMode())
	{
		if (other->GetOwner()->CompareTag("Player"))
		{
			//componentAnimation->SetParameter("IsActive", false);
			//componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
		}
	}
}

void ElevatorCore::DisableAllInteractions()
{
	//currentPlayer->SetParent(elevator);
	currentPlayer->GetComponentInternal<ComponentRigidBody>()->SetIsStatic(true);

	PlayerManagerScript* manager = currentPlayer->GetComponentInternal<PlayerManagerScript>();
	manager->ParalyzePlayer(true);
}

void ElevatorCore::EnableAllInteractions()
{
	//currentPlayer->SetParent(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot());
	currentPlayer->GetComponentInternal<ComponentRigidBody>()->SetIsStatic(false);

	PlayerManagerScript* manager = currentPlayer->GetComponentInternal<PlayerManagerScript>();
	manager->ParalyzePlayer(false);
}