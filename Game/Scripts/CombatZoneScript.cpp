#include "StdAfx.h"
#include "CombatZoneScript.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Scene/Scene.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "../Scripts/CameraControllerScript.h"

#include "GameObject/GameObject.h"

REGISTERCLASS(CombatZoneScript);

CombatZoneScript::CombatZoneScript() : Script(), componentAudio(nullptr), enemiesToDefeat(1.0)
{
	REGISTER_FIELD(enemiesToDefeat, float);
	REGISTER_FIELD(isBoss, bool);
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
	/*
	if (childWithRigid == std::end(children))
	{
		LOG_ERROR("Expected one of {}'s children to have a ComponentRigidBody, but none was found", GetOwner());
		throw ComponentNotFoundException("ComponentRigidBody not found in children");
	}
	componentRigidBody = (*childWithRigid)->GetComponent<ComponentRigidBody>();
	componentRigidBody->Disable();
	*/
	App->GetModule<ModulePlayer>()->SetInBossCombat(false);
}

void CombatZoneScript::Update(float deltaTime)
{
}

void CombatZoneScript::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter", other->GetOwner());
	
	if (other->GetOwner()->CompareTag("Player"))
	{
		if (isBoss)
		{
			App->GetModule<ModulePlayer>()->SetInBossCombat(true);
		}
		else
		{
			App->GetModule<ModulePlayer>()->SetInCombat(true);
			App->GetModule<ModulePlayer>()->SetEnemiesToDefeat(enemiesToDefeat);
		}

		std::vector<ComponentScript*> gameObjectScripts = owner->GetComponents<ComponentScript>();

		for (ComponentScript* script : gameObjectScripts)
		{
			if (script->GetConstructName() == "CombatZoneScript")
			{
				script->Disable();
			}
		}
	}
}