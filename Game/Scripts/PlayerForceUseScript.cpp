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
maxDistanceForce(20.0f), minDistanceForce(6.0f), maxTimeForce(15.0f), 
currentTimeForce(0.0f), ownerLastForward(float3(0,0,0))
{
	REGISTER_FIELD(maxDistanceForce, float);
	REGISTER_FIELD(maxTimeForce, float);
}

PlayerForceUseScript::~PlayerForceUseScript()
{
}

void PlayerForceUseScript::Start()
{
	currentTimeForce = maxTimeForce;
}

void PlayerForceUseScript::Update(float deltaTime)
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	ComponentTransform* transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));


	if (input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE && !gameObjectAttached && currentTimeForce > 14.0f)
	{
		RaycastHit hit;
		Ray ray(transform->GetGlobalPosition(), transform->GetGlobalForward());
		LineSegment line(ray, App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		if (Physics::RaycastToTag(line, hit, owner, tag))
		{	
			gameObjectAttached = hit.gameObject;
			ComponentTransform* hittedTransform = static_cast<ComponentTransform*>
				(gameObjectAttached->GetComponent(ComponentType::TRANSFORM));
			distancePointGameObjectAttached = transform->GetGlobalPosition().Distance(hittedTransform->GetGlobalPosition());

			if (distancePointGameObjectAttached > maxDistanceForce)
			{
				gameObjectAttached = nullptr;
				return;
			}
			else if (distancePointGameObjectAttached < minDistanceForce)
			{
				distancePointGameObjectAttached = minDistanceForce;
			}
			ownerLastForward = hittedTransform->GetGlobalForward();

			ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
			rigidBody->SetKpForce(50.0f);
			rigidBody->SetKpTorque(50.0f);
		}
	}
	else if ((input->GetKey(SDL_SCANCODE_E) == KeyState::IDLE && gameObjectAttached) || currentTimeForce < 0.0f)
	{
		ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		gameObjectAttached = nullptr;
		rigidBody->DisablePositionController();
		//rigidBody->DisableRotationController();
	}

	if (gameObjectAttached)
	{
		if (input->IsMouseWheelScrolled())
		{
			distancePointGameObjectAttached += (input->GetMouseWheel().y);

			distancePointGameObjectAttached = std::min(distancePointGameObjectAttached, maxDistanceForce);
			distancePointGameObjectAttached = std::max(distancePointGameObjectAttached, minDistanceForce);
		}
		// Get next position of the gameObject
		float3 nextPosition = transform->GetGlobalForward();
		nextPosition.Normalize();
		nextPosition *= distancePointGameObjectAttached;
		nextPosition += transform->GetGlobalPosition();

		// Get next rotation of game object
		/*ComponentTransform* hittedTransform = static_cast<ComponentTransform*>
			(gameObjectAttached->GetComponent(ComponentType::TRANSFORM));
		Quat targetRotation =
			Quat::RotateFromTo((transform->GetGlobalForward()).Normalized(), (ownerLastForward).Normalized());*/

		// Set position and rotation
		ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		rigidBody->SetPositionTarget(nextPosition);
		//rigidBody->SetRotationTarget(targetRotation);

		ownerLastForward = transform->GetGlobalForward();
		currentTimeForce -= deltaTime;
	}
	else if(currentTimeForce < maxTimeForce)
	{
		currentTimeForce = std::min(maxTimeForce, currentTimeForce + (deltaTime * 4));
	}
	ENGINE_LOG("Current time force : % f", currentTimeForce)
}