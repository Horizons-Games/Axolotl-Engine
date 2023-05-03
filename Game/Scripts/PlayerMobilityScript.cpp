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
		if (moveScript)
		{
			moveScript->PreUpdate();
		}
		Rotate();
	}
	
}

void PlayerMobilityScript::Rotate()
{
	float deltaTime = App->GetDeltaTime();
	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float3 newRot = trans->GetRotationXYZ();
	ModuleInput* input = App->GetModule<ModuleInput>();
	newRot.y += -input->GetMouseMotion().x * deltaTime;
	trans->SetRotation(newRot);
	trans->UpdateTransformMatrices();


	//Corroborate that you don't fuse with a owner
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
	float3 direction = (points[1] - points[0]).Normalized();
	if (collider->IsColliding(frontPoints, -direction, trans->GetLocalAABB().Size().z * 0.5f))
	{
		float deltaTime = App->GetDeltaTime();
		ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
		float3 newRot = trans->GetRotationXYZ();
		newRot.y += input->GetMouseMotion().x * deltaTime;
		trans->SetRotation(newRot);
		trans->UpdateTransformMatrices();
	}
}