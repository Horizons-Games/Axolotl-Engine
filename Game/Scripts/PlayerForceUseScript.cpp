#include "PlayerForceUseScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Components/ComponentTransform.h"

#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"

REGISTERCLASS(PlayerForceUseScript);

PlayerForceUseScript::PlayerForceUseScript() : Script(), gameObjectAttached(nullptr),
gameObjectAttachedParent(nullptr), tag("Forzable")
{
}

PlayerForceUseScript::~PlayerForceUseScript()
{
}

void PlayerForceUseScript::Start()
{
}

void PlayerForceUseScript::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));


	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE && !gameObjectAttached)
	{
		RaycastHit hit;
		Ray ray(trans->GetPosition(), trans->GetGlobalForward());
		LineSegment line(ray, App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		bool hasHit = Physics::RaycastToTag(line, hit, owner, tag);
		if (Physics::RaycastToTag(line, hit, owner, tag))
		{
			gameObjectAttached = hit.gameObject;
			gameObjectAttachedParent = gameObjectAttached->GetParent();
			gameObjectAttached->SetParent(owner);
		}
	}
	else if (input->GetKey(SDL_SCANCODE_Q) == KeyState::IDLE && gameObjectAttached)
	{
		gameObjectAttached->SetParent(gameObjectAttachedParent);
		gameObjectAttached = nullptr;
	}

	if (gameObjectAttached)
	{
		ComponentTransform* trans2 = static_cast<ComponentTransform*>(gameObjectAttached->GetComponent(ComponentType::TRANSFORM));

		ENGINE_LOG("%f, %f, %f", trans2->GetPosition().x, trans2->GetPosition().y, trans2->GetPosition().z);
	}
	ENGINE_LOG("%f, %f, %f", trans->GetPosition().x, trans->GetPosition().y, trans->GetPosition().z);

}