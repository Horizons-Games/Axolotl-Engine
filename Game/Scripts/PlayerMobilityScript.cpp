#include "PlayerMobilityScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentAudioSource.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(PlayerMobilityScript);

PlayerMobilityScript::PlayerMobilityScript() : Script(), componentPlayer(nullptr), 
												moveScript(nullptr), rotateScript(nullptr)
{
}

void PlayerMobilityScript::Start()
{

	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "PlayerMoveScript")
		{
			moveScript = gameObjectScripts[i];
			moveScript->GetScript()->Start();
		}

		else if (gameObjectScripts[i]->GetConstructName() == "PlayerRotationScript")
		{
			rotateScript = gameObjectScripts[i];
			rotateScript->GetScript()->Start();
		}
	}

	componentPlayer = static_cast<ComponentPlayer*>(owner->GetComponent(ComponentType::PLAYER));
}

void PlayerMobilityScript::PreUpdate(float deltaTime)
{
	
	if (!componentPlayer->IsStatic() && App->GetModule<ModuleCamera>()->GetSelectedPosition() == 0
		&& !SDL_ShowCursor(SDL_QUERY))
	{
		if (rotateScript)
		{
			rotateScript->GetScript()->PreUpdate(deltaTime);
		}

		if (moveScript)
		{
			canDash = false;
			nextDash += 5000;
		}	
	}

	// Cooldown Dash
	if (nextDash > 0 && nextDash < SDL_GetTicks())
	{
		canDash = true;
		nextDash = 0;
	}

	// Run, duplicate the speed
	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		size *= 2;
	}

	// Crouch
	if (input->GetKey(SDL_SCANCODE_LCTRL) != KeyState::IDLE && !isCrouch)
	{
		isCrouch = true;
		trans->SetScale(trans->GetScale() / 2);
		GameObject::GameObjectView children = owner->GetChildren();
		for (auto child : children) 
		{
			if (child->GetComponent(ComponentType::CAMERA))
			{
				ComponentTransform* childTrans = static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
				childTrans->SetScale(childTrans->GetScale() * 2);
			}

		}
		size /= 4.f;
	}
	else if (input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::IDLE && isCrouch)
	{
		isCrouch = false;
		trans->SetScale(trans->GetScale() * 2);
		GameObject::GameObjectView children = owner->GetChildren();
		for (auto child : children)
		{
			if (child->GetComponent(ComponentType::CAMERA))
			{
				ComponentTransform* childTrans = static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
				childTrans->SetScale(childTrans->GetScale() / 2);
			}
		}
	}
}