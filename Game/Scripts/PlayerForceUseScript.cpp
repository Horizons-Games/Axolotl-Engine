#include "PlayerForceUseScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentCamera.h"
#include "DataModels/Camera/CameraGameObject.h"

#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"

REGISTERCLASS(PlayerForceUseScript);

PlayerForceUseScript::PlayerForceUseScript() : Script(), gameObjectAttached(nullptr),
gameObjectAttachedParent(nullptr), tag("Forzable"), hitPointGameObjectAttached(float3(0, 0, 0))
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
		}
	}
	else if (input->GetKey(SDL_SCANCODE_Q) == KeyState::IDLE && gameObjectAttached)
	{
		gameObjectAttached = nullptr;
	}

	if (gameObjectAttached)
	{
		ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		ModuleInput* input = App->GetModule<ModuleInput>();
		btRigidBody* btRb = rigidBody->GetRigidBody();

		ComponentTransform* transform = static_cast<ComponentTransform*>(gameObjectAttached->GetComponent(ComponentType::TRANSFORM));

		ComponentCamera* camera = static_cast<ComponentCamera*>(owner->GetComponent(ComponentType::CAMERA));
		float3 nextPosition = transform->GetGlobalPosition() - trans->GetGlobalPosition();
		nextPosition.Normalize();

		float3 nextDirection = Quat::SlerpVector(camera->GetCamera()->GetFrustum()->Front(), nextPosition, deltaTime);

		btTransform trans;
		trans = btRb->getWorldTransform();
		trans.setOrigin(btVector3(nextDirection.x, nextDirection.y, nextDirection.z));
		btRb->setWorldTransform(trans);

	}
}