#include "PlayerForceUseScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"

REGISTERCLASS(PlayerForceUseScript);

PlayerForceUseScript::PlayerForceUseScript() : Script(), gameObjectAttached(nullptr),
gameObjectAttachedParent(nullptr), tag("Forzable"), gameObjectAttachedGravity(btVector3(0, 0, 0))
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
		Ray ray(trans->GetGlobalPosition(), trans->GetGlobalForward());
		LineSegment line(ray, App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		bool hasHit = Physics::RaycastToTag(line, hit, owner, tag);
		if (Physics::RaycastToTag(line, hit, owner, tag))
		{
			gameObjectAttached = hit.gameObject;
			gameObjectAttachedParent = gameObjectAttached->GetParent();
			gameObjectAttached->SetParent(owner);

			ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>
				(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
			gameObjectAttachedGravity = rigidBody->GetGravity();
			rigidBody->SetGravity(btVector3(0,0,0));
			
		}
	}
	else if (input->GetKey(SDL_SCANCODE_Q) == KeyState::IDLE && gameObjectAttached)
	{
		gameObjectAttached->SetParent(gameObjectAttachedParent);
		ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>
			(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		rigidBody->SetGravity(gameObjectAttachedGravity);
		gameObjectAttached = nullptr;
	}
}