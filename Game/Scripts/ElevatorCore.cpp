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


#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(ElevatorCore);

ElevatorCore::ElevatorCore() : Script(),
componentAudio(nullptr), activeState(ActiveActions::INACTIVE), positionState(PositionState::UP),
goTransform(nullptr), go(nullptr)
{
	REGISTER_FIELD(elevator, GameObject*);
	REGISTER_FIELD(finalPos, float);
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
	finalUpPos = 0;
	//bixPrefab = App->GetModule<ModulePlayer>()->GetPlayer();
}

void ElevatorCore::Update(float deltaTime)
{
	if (activeState == ActiveActions::ACTIVE) 
	{
		float3 pos = transform->GetGlobalPosition();
		float3 goPos = goTransform->GetGlobalPosition();
		btVector3 triggerOrigin = triggerEntrance->GetRigidBodyOrigin();

		if (positionState == PositionState::UP)
		{
			pos.y -= 0.1f;
			goPos.y -= 0.1f;
			float newYTrigger = triggerOrigin.getY() - 0.1f;
			triggerOrigin.setY(newYTrigger);
		}
		else 
		{
			pos.y += 0.1f;
			goPos.y += 0.1f;
			float newYTrigger = triggerOrigin.getY() + 0.1f;
			triggerOrigin.setY(newYTrigger);
		}
		
		transform->SetGlobalPosition(pos);
		goTransform->SetGlobalPosition(goPos);

		transform->RecalculateLocalMatrix();
		transform->UpdateTransformMatrices();

		goTransform->RecalculateLocalMatrix();
		goTransform->UpdateTransformMatrices();

		
		triggerEntrance->SetRigidBodyOrigin(triggerOrigin);
		elevator->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
		go->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
		
		if (pos.y <= finalPos)
		{
			positionState = PositionState::DOWN;
			activeState = ActiveActions::INACTIVE;

			if (go->CompareTag("Player"))
			{
				SetDisableInteractions(false);
			}
			else
			{
				SetDisableInteractionsEnemies(go, false);
			}
		}
		
		else if (pos.y >= finalUpPos)
		{
			positionState = PositionState::UP;
			activeState = ActiveActions::INACTIVE;

			if (go->CompareTag("Player"))
			{
				SetDisableInteractions(false);
			}
			else
			{
				SetDisableInteractionsEnemies(go, false);
			}
		}
	}
}

void ElevatorCore::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter of ElevatorCore", other->GetOwner());
	if (!App->GetModule<ModuleScene>()->GetLoadedScene()->GetCombatMode() 
		&& activeState == ActiveActions::INACTIVE)
	{
		if (other->GetOwner()->CompareTag("Player"))
		{
			go = other->GetOwner();
			goTransform = go->GetComponentInternal<ComponentTransform>();
			PlayerActions currentAction = go->GetComponent<PlayerManagerScript>()->GetPlayerState();
			bool isJumping = currentAction == PlayerActions::JUMPING ||
				currentAction == PlayerActions::DOUBLEJUMPING ||
				currentAction == PlayerActions::FALLING;

			if (!isJumping)
			{
				//componentAnimation->SetParameter("IsActive", true);
				componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
				activeState = ActiveActions::ACTIVE;

				SetDisableInteractions(true);
			}
			
		}
		else if (other->GetOwner()->CompareTag("Enemy"))
		{
			go = other->GetOwner();
			goTransform = go->GetComponentInternal<ComponentTransform>();
			SetDisableInteractionsEnemies(other->GetOwner(), true);
		}
	}
}

void ElevatorCore::SetDisableInteractions(bool interactions)
{
	go->GetComponentInternal<ComponentRigidBody>()->SetStatic(interactions);

	PlayerManagerScript* manager = go->GetComponentInternal<PlayerManagerScript>();
	manager->ParalyzePlayer(interactions);
}

void ElevatorCore::SetDisableInteractionsEnemies(const GameObject* enemy, bool interactions)
{
	if (enemy->HasComponent<EnemyVenomiteScript>())
	{
		activeState = ActiveActions::ACTIVE;
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
		enemy->GetComponentInternal<ComponentRigidBody>()->SetStatic(interactions);
		enemy->GetComponent<EnemyVenomiteScript>()->ParalyzeEnemy(interactions);
		goTransform = enemy->GetComponent<ComponentTransform>();
	}
	else if (enemy->HasComponent<EnemyDroneScript>())
	{
		activeState = ActiveActions::ACTIVE;				
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
		enemy->GetComponentInternal<ComponentRigidBody>()->SetStatic(interactions);
		enemy->GetComponent<EnemyDroneScript>()->ParalyzeEnemy(interactions);
		goTransform = enemy->GetComponent<ComponentTransform>();
	}
}