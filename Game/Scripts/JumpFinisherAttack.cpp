#include "StdAfx.h"
#include "JumpFinisherAttack.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Scripting/ScriptFactory.h"
#include "Components/ComponentScript.h"

#include "../Scripts/JumpFinisherArea.h"
#include "../Scripts/JumpFinisherAttackBullet.h"

REGISTERCLASS(JumpFinisherAttack);

JumpFinisherAttack::JumpFinisherAttack() : Script(), cooldown(6.0f), currentCooldown(0.0f), input(nullptr)
{
	REGISTER_FIELD(cooldown, float);
	REGISTER_FIELD(forceArea, JumpFinisherArea*);
	REGISTER_FIELD(forceAttackBullet, GameObject*);
}

void JumpFinisherAttack::Start()
{
	input = App->GetModule<ModuleInput>();
}

void JumpFinisherAttack::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE && currentCooldown <= 0) // Bix jump finisher
	{
		currentCooldown = cooldown;
		//JumpFinisherArea* forceAreaComponent = forceArea->GetComponent<JumpFinisherArea>();
		//forceAreaComponent->PushEnemies();
		forceArea->PushEnemies();
	}

	else if (input->GetKey(SDL_SCANCODE_T) != KeyState::IDLE && currentCooldown <= 0) // Allura jump finisher
	{
		currentCooldown = cooldown;
		ShootForceBullet();
	}

	else
	{
		if (currentCooldown > 0)
		{
			currentCooldown -= deltaTime;
			currentCooldown = std::max(0.0f, currentCooldown);
		}
	}
}

void JumpFinisherAttack::ShootForceBullet()
{
	// Duplicate force bullet
	GameObject* newForceBullet = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject(forceAttackBullet->GetName(), forceAttackBullet, owner);

	// Launch it in front of Allura
}