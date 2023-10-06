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
#include "EnemyDroneScript.h"
#include "EnemyVenomiteScript.h"
#include "HealthSystem.h"


#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(ElevatorCore);

ElevatorCore::ElevatorCore() : Script(),
componentAudio(nullptr), activeState(ActiveActionsElevator::INACTIVE), positionState(PositionState::UP),
goTransform(nullptr), go(nullptr)
{
	REGISTER_FIELD(elevator, GameObject*);
	REGISTER_FIELD(finalPos, float);
	REGISTER_FIELD(coolDown, float);
	REGISTER_FIELD(speed, float);
	REGISTER_FIELD(miniBossHealth, HealthSystem*);
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
	finalUpPos = transform->GetGlobalPosition().y;
	finalPos = finalUpPos + finalPos;

	currentTime = 0.0f;
}

void ElevatorCore::Update(float deltaTime)
{
	if (activeState == ActiveActionsElevator::ACTIVE)
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

	else if (activeState == ActiveActionsElevator::ACTIVE_AUTO)
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
		/*if (goTransform->GetGlobalPosition().y < finalPos)
		{
			if ((!miniBossHealth->EntityIsAlive() && positionState == PositionState::UP) ||
				(miniBossHealth->EntityIsAlive() && positionState == PositionState::DOWN))
			{
				activeState = ActiveActions::ACTIVE_AUTO;
			}
			
		}
		*/
		if (currentTime >= 0.0f)
		{
			currentTime -= deltaTime;
		}

	}
}

void ElevatorCore::MoveUpElevator(bool isGOInside, float deltaTime)
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
		activeState = ActiveActionsElevator::INACTIVE;
		booked = false;
		currentTime = coolDown;
		if (isGOInside)
		{
			if (go->CompareTag("Player"))
			{
				SetDisableInteractions(false);
			}
			else if (go->CompareTag("Enemy"))
			{
				SetDisableInteractionsEnemies(go, false, false, false);
			}
		}
	}

	if (isGOInside)
	{
		float3 goPos = goTransform->GetGlobalPosition();
		goPos.y += deltaTime * speed;

		goTransform->SetGlobalPosition(goPos);
		goTransform->RecalculateLocalMatrix();
		goTransform->UpdateTransformMatrices();
		go->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();

	}

}

void ElevatorCore::MoveDownElevator(bool isGOInside, float deltaTime)
{
	float3 pos = transform->GetGlobalPosition();
	float3 goPos = goTransform->GetGlobalPosition();
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
		activeState = ActiveActionsElevator::INACTIVE;
		booked = false;
		currentTime = coolDown;
		if (isGOInside)
		{
			if (go->CompareTag("Player"))
			{
				SetDisableInteractions(false);
			}
			else if (go->CompareTag("Enemy"))
			{
				SetDisableInteractionsEnemies(go, false, false, false);
			}
		}
	}

	if (isGOInside)
	{
		float3 goPos = goTransform->GetGlobalPosition();
		goPos.y -= deltaTime * speed;

		goTransform->SetGlobalPosition(goPos);
		goTransform->RecalculateLocalMatrix();
		goTransform->UpdateTransformMatrices();
		go->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
	}
}

void ElevatorCore::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter of ElevatorCore", other->GetOwner());
	if (!App->GetModule<ModuleScene>()->GetLoadedScene()->GetCombatMode() 
		&& activeState == ActiveActionsElevator::INACTIVE)
	{
		if (other->GetOwner()->CompareTag("Player"))
		{
			go = other->GetOwner();
			goTransform = go->GetComponentInternal<ComponentTransform>();
			PlayerActions currentAction = go->GetComponent<PlayerManagerScript>()->GetPlayerState();

			bool isJumping = currentAction == PlayerActions::JUMPING ||
				currentAction == PlayerActions::DOUBLEJUMPING ||
				currentAction == PlayerActions::FALLING;

			if (!isJumping && currentTime <= 0.0f)
			{
				booked = true;
				//componentAnimation->SetParameter("IsActive", true);
				componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
				activeState = ActiveActionsElevator::ACTIVE;

				SetDisableInteractions(true);
			}
			
		}
		else if (other->GetOwner()->CompareTag("Enemy") && currentTime <= 0.0f)
		{
			go = other->GetOwner();
			goTransform = go->GetComponentInternal<ComponentTransform>();
			SetDisableInteractionsEnemies(other->GetOwner(), true, true, true);
		}
	}
}

void ElevatorCore::SetDisableInteractions(bool interactions)
{
	go->GetComponentInternal<ComponentRigidBody>()->SetStatic(interactions);

	PlayerManagerScript* manager = go->GetComponentInternal<PlayerManagerScript>();
	manager->ParalyzePlayer(interactions);
}

void ElevatorCore::ActiveAuto()
{
	activeState = ActiveActionsElevator::ACTIVE_AUTO;
}

void ElevatorCore::SetDisableInteractionsEnemies(const GameObject* enemy, bool interactions,
	bool activeElevator, bool setStaticRigidBody)
{
	if (enemy->HasComponent<EnemyVenomiteScript>())
	{
		if (activeElevator)
		{
			activeState = ActiveActionsElevator::ACTIVE;
		}
		enemy->GetComponentInternal<ComponentRigidBody>()->SetStatic(setStaticRigidBody);
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
		enemy->GetComponent<EnemyVenomiteScript>()->ParalyzeEnemy(interactions);
	}
	else if (enemy->HasComponent<EnemyDroneScript>())
	{
		if (activeElevator)
		{
			activeState = ActiveActionsElevator::ACTIVE;
		}		
		enemy->GetComponentInternal<ComponentRigidBody>()->SetStatic(setStaticRigidBody);
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
		enemy->GetComponent<EnemyDroneScript>()->ParalyzeEnemy(interactions);
	}
}

bool ElevatorCore::GetElevatorPos(const PositionState pos) const
{
	return ((positionState == pos) && (0 > currentTime));
}

bool ElevatorCore::GetBooked() const
{
	return booked;
}

void ElevatorCore::SetBooked(bool nbooked)
{
	booked = nbooked;
}