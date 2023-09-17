#include "StdAfx.h"
#include "ElevatorCore.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "PlayerManagerScript.h";
#include "PlayerMoveScript.h"
#include "PlayerJumpScript.h"
#include "PlayerAttackScript.h"


#include "Scene/Scene.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"


REGISTERCLASS(ElevatorCore);

ElevatorCore::ElevatorCore() : Script(),
componentAudio(nullptr), activeState(ActiveActions::INACTIVE)
{
	REGISTER_FIELD(elevator, GameObject*);
	REGISTER_FIELD(bixPrefab, GameObject*)
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
	//componentRigidBody->Disable();
}

void ElevatorCore::Update(float deltaTime)
{
	if (activeState == ActiveActions::ACTIVE) 
	{
		ComponentTransform* transform = elevator->GetComponentInternal<ComponentTransform>();
		float3 pos = transform->GetGlobalPosition();
		pos.y -= 0.1;
		transform->SetGlobalPosition(pos);

		transform->RecalculateLocalMatrix();
		transform->UpdateTransformMatrices();
		elevator->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();
		bixPrefab->GetComponentInternal<ComponentRigidBody>()->UpdateRigidBody();

		DisableAllInteractions();


		LOG_DEBUG("POS.Y: {}, FINAL POS: {}", pos.y, finalPos);
		
		if (pos.y < finalPos)
		{
			activeState = ActiveActions::INACTIVE;
		}


		componentRigidBody->Disable();
		
	}

	else
	{
		bixPrefab->SetParent(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot());
		bixPrefab->GetComponentInternal<ComponentRigidBody>()->SetStatic(false);
		EnableAllInteractions();
	}
}


void ElevatorCore::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter of ElevatorCore", other->GetOwner());
	if (!App->GetModule<ModuleScene>()->GetLoadedScene()->GetCombatMode())
	{
		if (other->GetOwner()->CompareTag("Player"))
		{
	//		componentAnimation->SetParameter("IsActive", true);
			//componentRigidBody->Disable();
			componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
			activeState = ActiveActions::ACTIVE;

			bixPrefab->SetParent(elevator);
			bixPrefab->GetComponentInternal<ComponentRigidBody>()->SetStatic(true);
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
			//// Until the trigger works 100% of the time better cross a closed door than be closed forever
			//componentRigidBody->Enable();
			//componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
		}
	}
}

void ElevatorCore::DisableAllInteractions()
{
	PlayerManagerScript* manager = bixPrefab->GetComponentInternal<PlayerManagerScript>();
	PlayerJumpScript* jump = bixPrefab->GetComponentInternal<PlayerJumpScript>();
	PlayerMoveScript* move = bixPrefab->GetComponentInternal<PlayerMoveScript>();
	PlayerAttackScript* attack = bixPrefab->GetComponentInternal<PlayerAttackScript>();
	manager->Disable();
	jump->Disable();
	move->Disable();
	attack->Disable();
}

void ElevatorCore::EnableAllInteractions()
{
	PlayerManagerScript* manager = bixPrefab->GetComponentInternal<PlayerManagerScript>();
	PlayerJumpScript* jump = bixPrefab->GetComponentInternal<PlayerJumpScript>();
	PlayerMoveScript* move = bixPrefab->GetComponentInternal<PlayerMoveScript>();
	PlayerAttackScript* attack = bixPrefab->GetComponentInternal<PlayerAttackScript>();
	manager->Enable();
	jump->Enable();
	move->Enable();
	attack->Enable();
}