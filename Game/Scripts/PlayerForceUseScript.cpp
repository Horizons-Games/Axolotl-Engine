#include "PlayerForceUseScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentAnimation.h"
#include "DataModels/Camera/CameraGameObject.h"

#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"

#include "PlayerRotationScript.h"
#include "PlayerCameraRotationVerticalScript.h"
#include "PlayerMoveScript.h"
#include "HealthSystem.h"

REGISTERCLASS(PlayerForceUseScript);

PlayerForceUseScript::PlayerForceUseScript() : Script(), gameObjectAttached(nullptr),
gameObjectAttachedParent(nullptr), tag("Forzable"), distancePointGameObjectAttached(0.0f),
maxDistanceForce(20.0f), minDistanceForce(6.0f), maxTimeForce(15.0f), 
currentTimeForce(0.0f), breakForce(false), componentAnimation(nullptr), healthScript(nullptr)
{
	REGISTER_FIELD(maxDistanceForce, float);
	REGISTER_FIELD(maxTimeForce, float);
}

PlayerForceUseScript::~PlayerForceUseScript()
{
}

void PlayerForceUseScript::Start()
{
	componentAnimation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));

	currentTimeForce = maxTimeForce;

	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetParent()->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);
	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "PlayerRotationScript")
		{
			rotationHorizontalScript = static_cast<PlayerRotationScript*>(gameObjectScripts[i]->GetScript());
		}
		else if (gameObjectScripts[i]->GetConstructName() == "PlayerMoveScript")
		{
			moveScript = static_cast<PlayerMoveScript*>(gameObjectScripts[i]->GetScript());
		}
		else if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
		{
			healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
		}
	}

	gameObjectScripts =
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);
	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "PlayerCameraRotationVerticalScript")
		{
			rotationVerticalScript = static_cast<PlayerCameraRotationVerticalScript*>(gameObjectScripts[i]->GetScript());
		}
	}
}

void PlayerForceUseScript::Update(float deltaTime)
{
	if (!healthScript->EntityIsAlive())
	{
		return;
	}

	const ModuleInput* input = App->GetModule<ModuleInput>();
	const ComponentTransform* transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	if (input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE && !gameObjectAttached && currentTimeForce > 14.0f)
	{
		//componentAnimation->SetParameter("IsUsingForce", true);
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

			if (rotationHorizontalScript)
			{
				lastHorizontalSensitivity = rotationHorizontalScript->GetField<float>("RotationSensitivity")->getter();
				rotationHorizontalScript->GetField<float>("RotationSensitivity")->setter(lastHorizontalSensitivity / 2.0f);
			}

			if (rotationVerticalScript)
			{
				lastVerticalSensitivity = rotationVerticalScript->GetField<float>("RotationSensitivity")->getter();
				rotationVerticalScript->GetField<float>("RotationSensitivity")->setter(lastVerticalSensitivity / 2.0f);
			}

			if (moveScript)
			{
				lastMoveSpeed = moveScript->GetField<float>("Speed")->getter();
				moveScript->GetField<float>("Speed")->setter(lastMoveSpeed / 2.0f);
			}

			ComponentRigidBody* rigidBody = 
				static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
			rigidBody->SetKpForce(50.0f);
			rigidBody->SetKpTorque(50.0f);
		}
	}
	else if ((input->GetKey(SDL_SCANCODE_E) == KeyState::IDLE 
		&& gameObjectAttached) 
		|| currentTimeForce < 0.0f 
		|| breakForce)
	{
		//componentAnimation->SetParameter("IsUsingForce", false);

		ComponentRigidBody* rigidBody = 
			static_cast<ComponentRigidBody*>(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		gameObjectAttached = nullptr;
		rigidBody->DisablePositionController();
		rigidBody->DisableRotationController();

		if (rotationHorizontalScript)
		{
			rotationHorizontalScript->GetField<float>("RotationSensitivity")->setter(lastHorizontalSensitivity);
		}

		if(rotationVerticalScript)
		{
			rotationVerticalScript->GetField<float>("RotationSensitivity")->setter(lastVerticalSensitivity);
		}

		if (moveScript)
		{
			moveScript->GetField<float>("Speed")->setter(lastMoveSpeed);
		}

		breakForce = false;
	}

	if (gameObjectAttached)
	{
		ComponentRigidBody* hittedRigidBody = static_cast<ComponentRigidBody*>
			(gameObjectAttached->GetComponent(ComponentType::RIGIDBODY));
		btRigidBody* hittedbtRb = hittedRigidBody->GetRigidBody();
		ComponentTransform* hittedTransform = static_cast<ComponentTransform*>
			(gameObjectAttached->GetComponent(ComponentType::TRANSFORM));

		if (input->IsMouseWheelScrolled())
		{
			distancePointGameObjectAttached += (input->GetMouseWheel().y) / 3.0f;
			distancePointGameObjectAttached = std::min(distancePointGameObjectAttached, maxDistanceForce);
			distancePointGameObjectAttached = std::max(distancePointGameObjectAttached, minDistanceForce);
		}
		// Get next position of the gameObject
		float3 nextPosition = transform->GetGlobalForward();
		nextPosition.Normalize();
		nextPosition *= distancePointGameObjectAttached;
		nextPosition += transform->GetGlobalPosition();

		float currentDistance = hittedTransform->GetGlobalPosition().Distance(nextPosition);

		if (std::abs(currentDistance) > 1.5f && currentTimeForce < 14.5f)
		{
			breakForce = true;
			currentTimeForce = 10;
			return;
		}

		// Get next rotation of game object
		ComponentTransform* parentTransform = static_cast<ComponentTransform*>
			(owner->GetParent()->GetComponent(ComponentType::TRANSFORM));
	
		Quat targetRotation =
			Quat::RotateFromTo(hittedTransform->GetGlobalForward(),
				(parentTransform->GetGlobalPosition() - hittedTransform->GetGlobalPosition()).Normalized());

		// Set position and rotation
		hittedRigidBody->SetPositionTarget(nextPosition);
		hittedRigidBody->SetRotationTarget(targetRotation);

		currentTimeForce -= deltaTime;
	}
	else if(currentTimeForce < maxTimeForce)
	{
		currentTimeForce = std::min(maxTimeForce, currentTimeForce + (deltaTime * 4));
	}
}