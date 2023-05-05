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

PlayerMobilityScript::~PlayerMobilityScript()
{
	delete componentPlayer;
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
		if (moveScript)
		{
			moveScript->PreUpdate();
		}

		if (rotateScript)
		{
			rotateScript->PreUpdate();
		}
	}
}