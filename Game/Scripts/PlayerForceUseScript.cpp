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
gameObjectAttachedParent(nullptr), tag("Forzable"), distancePointGameObjectAttached(0.0f),
ownerLastForward(float3(0,0,0)), gameObjectAttachedLastForward(float3(0,0,0))
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
	ComponentTransform* transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));


	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE && !gameObjectAttached)
	{
		RaycastHit hit;
		Ray ray(transform->GetGlobalPosition(), transform->GetGlobalForward());
		LineSegment line(ray, App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		bool hasHit = Physics::RaycastToTag(line, hit, owner, tag);
		if (Physics::RaycastToTag(line, hit, owner, tag))
		{
			gameObjectAttached = hit.gameObject;
			ComponentTransform* hittedTransform = static_cast<ComponentTransform*>
				(gameObjectAttached->GetComponent(ComponentType::TRANSFORM));
			distancePointGameObjectAttached = transform->GetGlobalPosition().Distance(hittedTransform->GetGlobalPosition());

			ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
			rigidBody->SetKpTorque(20.0f);

			// set first rotation
			ownerLastForward = transform->GetGlobalForward();
			gameObjectAttachedLastForward = hittedTransform->GetGlobalForward();
		}
	}
	else if (input->GetKey(SDL_SCANCODE_Q) == KeyState::IDLE && gameObjectAttached)
	{
		ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		gameObjectAttached = nullptr;
		rigidBody->DisablePositionController();
		rigidBody->DisableRotationController();
	}

	if (gameObjectAttached)
	{
		// Get next position of the gameObject
		float3 nextPosition = transform->GetGlobalForward();
		nextPosition.Normalize();
		nextPosition *= distancePointGameObjectAttached;
		nextPosition += transform->GetGlobalPosition();

		// Get next angle of the gameObject
		ComponentTransform* hittedTransform = static_cast<ComponentTransform*>
			(gameObjectAttached->GetComponent(ComponentType::TRANSFORM));

		float3 ownerNewForward = transform->GetGlobalForward();
		//ownerNewForward.Normalize();
		//ownerLastForward.Normalize();

		float3 ownerDiffForward = ownerLastForward - ownerNewForward;
		gameObjectAttachedLastForward += ownerDiffForward;

		Quat targetRotation = Quat::RotateFromTo(hittedTransform->GetGlobalForward(), gameObjectAttachedLastForward);


		ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		rigidBody->SetPositionTarget(nextPosition);
		rigidBody->SetRotationTarget(targetRotation);


		ownerLastForward = transform->GetGlobalForward();
		gameObjectAttachedLastForward = hittedTransform->GetGlobalForward();
	}
}