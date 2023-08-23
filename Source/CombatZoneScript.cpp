#include "StdAfx.h"
#include "CombatZoneScript.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentRigidBody.h"

#include "GameObject/GameObject.h"

REGISTERCLASS(CombatZoneScript);

CombatZoneScript::CombatZoneScript() : Script(), componentAudio(nullptr)
{
}

CombatZoneScript::~CombatZoneScript()
{
}

void CombatZoneScript::Start()
{
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
	componentRigidBody->Disable();
	enemiesToDefeat = 1;
}

void CombatZoneScript::Update(float deltaTime)
{
}

void CombatZoneScript::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter", other->GetOwner());
	
	if (other->GetOwner()->CompareTag("Player"))
	{
		componentRigidBody->Disable();
		App->GetModule<ModuleScene>()->GetLoadedScene()->SetEnemiesToDefeat(enemiesToDefeat);
		App->GetModule<ModuleScene>()->GetLoadedScene()->SetCombatMode(true);
		owner->Disable();
	}
}