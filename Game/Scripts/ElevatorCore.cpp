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
	finalUpPos = 0;
	bixPrefab = App->GetModule<ModulePlayer>()->GetPlayer();
	playerTransform = bixPrefab->GetComponent<ComponentTransform>();
}

void ElevatorCore::Update(float deltaTime)
{
	if (activeState == ActiveActions::ACTIVE) 
	{
		float3 pos = transform->GetGlobalPosition();
		float3 playerPos = playerTransform->GetGlobalPosition();
		btVector3 triggerOrigin = triggerEntrance->GetRigidBodyOrigin();

		if (positionState == PositionState::UP)
		{
			pos.y -= 0.1f;
			playerPos.y -= 0.1f;
			float newYTrigger = triggerOrigin.getY() - 0.1f;
			triggerOrigin.setY(newYTrigger);
		}
		else 
		{
			pos.y += 0.1f;
			playerPos.y += 0.1f;
			float newYTrigger = triggerOrigin.getY() + 0.1f;
			triggerOrigin.setY(newYTrigger);
		}
		
		transform->SetGlobalPosition(pos);
		playerTransform->SetGlobalPosition(playerPos);

		transform->RecalculateLocalMatrix();
		transform->UpdateTransformMatrices();

		playerTransform->RecalculateLocalMatrix();
		playerTransform->UpdateTransformMatrices();

		
		triggerEntrance->SetRigidBodyOrigin(triggerOrigin);
		elevator->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
		bixPrefab->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
		
		if (pos.y <= finalPos)
		{
			positionState = PositionState::DOWN;
			activeState = ActiveActions::INACTIVE;

			EnableAllInteractions();
		}
		
		else if (pos.y >= finalUpPos)
		{
			positionState = PositionState::UP;
			activeState = ActiveActions::INACTIVE;

			EnableAllInteractions();
		}
	}
}


void ElevatorCore::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter of ElevatorCore", other->GetOwner());
	if (!App->GetModule<ModuleScene>()->GetLoadedScene()->GetCombatMode())
	{
		if (other->GetOwner()->CompareTag("Player"))
		{

			PlayerActions currentAction = bixPrefab->GetComponent<PlayerManagerScript>()->GetPlayerState();
			bool isJumping = currentAction == PlayerActions::JUMPING ||
				currentAction == PlayerActions::DOUBLEJUMPING ||
				currentAction == PlayerActions::FALLING;

			if (!isJumping)
			{
				//componentAnimation->SetParameter("IsActive", true);
				componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
				activeState = ActiveActions::ACTIVE;

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
	//bixPrefab->SetParent(elevator);
	bixPrefab->GetComponentInternal<ComponentRigidBody>()->SetIsStatic(true);

	PlayerManagerScript* manager = bixPrefab->GetComponentInternal<PlayerManagerScript>();
	manager->ParalyzePlayer(true);
}

void ElevatorCore::EnableAllInteractions()
{
	//bixPrefab->SetParent(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot());
	bixPrefab->GetComponentInternal<ComponentRigidBody>()->SetIsStatic(false);

	PlayerManagerScript* manager = bixPrefab->GetComponentInternal<PlayerManagerScript>();
	manager->ParalyzePlayer(false);
}