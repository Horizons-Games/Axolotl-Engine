#include "PlayerForceUseScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "DataModels/Camera/CameraGameObject.h"

#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"
#include "Auxiliar/Audio/AudioData.h"

#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/PlayerRotationScript.h"
#include "../Scripts/CameraControllerScript.h"
#include "../Scripts/PlayerMoveScript.h"


#include "AxoLog.h"

REGISTERCLASS(PlayerForceUseScript);

PlayerForceUseScript::PlayerForceUseScript() : Script(), gameObjectAttached(nullptr),
	gameObjectAttachedParent(nullptr), tag("Forceable"), distancePointGameObjectAttached(0.0f),
	maxDistanceForce(20.0f), minDistanceForce(6.0f), maxTimeForce(15.0f), isForceActive(false),
	currentTimeForce(0.0f), breakForce(false), componentAnimation(nullptr), componentAudioSource(nullptr),
	playerManagerScript(nullptr)
{
	REGISTER_FIELD(maxDistanceForce, float);
	REGISTER_FIELD(maxTimeForce, float);

}

void PlayerForceUseScript::Start()
{
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	currentTimeForce = maxTimeForce;

	rotationHorizontalScript = owner->GetParent()->GetComponent<PlayerRotationScript>();
	playerManagerScript = owner->GetParent()->GetComponent<PlayerManagerScript>();
	moveScript = owner->GetComponent<PlayerMoveScript>();

	input = App->GetModule<ModuleInput>();
	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
}

void PlayerForceUseScript::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE && !gameObjectAttached && currentTimeForce > 14.0f)
	{
		componentAnimation->SetParameter("IsStartingForce", true);
		componentAnimation->SetParameter("IsStoppingForce", false);
		RaycastHit hit;
		btVector3 rigidBodyOrigin = rigidBody->GetRigidBodyOrigin();
		float3 origin = float3(rigidBodyOrigin.getX(), rigidBodyOrigin.getY(), rigidBodyOrigin.getZ());
		int raytries = 0;
		
		while (gameObjectAttached == nullptr && raytries < 4)
		{
			Ray ray(origin + float3(0.f, 1 * raytries, 0.f), transform->GetGlobalForward() );
			LineSegment line(ray, 300);
			raytries++;

			if (Physics::RaycastToTag(line, hit, owner, tag))
			{
				gameObjectAttached = hit.gameObject;
				ComponentTransform* hittedTransform = gameObjectAttached->GetComponent<ComponentTransform>();
				distancePointGameObjectAttached = transform->GetGlobalPosition().Distance(hit.hitPoint);
				ComponentRigidBody* rigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();
				objectStaticness = rigidBody->IsStatic();
				rigidBody->SetStatic(false);
				offsetFromPickedPoint = hittedTransform->GetGlobalPosition() - hit.hitPoint;
				pickedForward = transform->GetGlobalForward();


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
					lastHorizontalSensitivity = rotationHorizontalScript->GetHorizontalSensitivity();
					rotationHorizontalScript->SetHorizontalSensitivity(lastHorizontalSensitivity / 2.0f);
					lastVerticalSensitivity = rotationHorizontalScript->GetVerticalSensitivity();
					rotationHorizontalScript->SetVerticalSensitivity(lastVerticalSensitivity / 2.0f);
				}

				if (playerManagerScript)
				{
					lastMoveSpeed = playerManagerScript->GetPlayerSpeed();
					playerManagerScript->GetField<float>("PlayerSpeed")->setter(lastMoveSpeed / 2.0f);
				}


				rigidBody->SetKpForce(50.0f);
				rigidBody->SetKpTorque(50.0f);


			}
		}
	}

	else if ((input->GetKey(SDL_SCANCODE_E) == KeyState::IDLE
		&& gameObjectAttached)
		|| currentTimeForce < 0.0f
		|| breakForce)
	{
		ComponentRigidBody* rigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();
		gameObjectAttached = nullptr;
		rigidBody->DisablePositionController();
		rigidBody->DisableRotationController();
		rigidBody->SetStatic(objectStaticness);

		if (rotationHorizontalScript)
		{
			rotationHorizontalScript->GetField<float>("RotationSensitivity")->setter(lastHorizontalSensitivity);
		}

		if (rotationVerticalScript)
		{
			rotationVerticalScript->GetField<float>("RotationSensitivity")->setter(lastVerticalSensitivity);
		}

		if (playerManagerScript)
		{
			playerManagerScript->GetField<float>("PlayerSpeed")->setter(lastMoveSpeed);
			rotationHorizontalScript->SetHorizontalSensitivity(lastHorizontalSensitivity);
			rotationHorizontalScript->SetVerticalSensitivity(lastVerticalSensitivity);
		}

		if (isForceActive)
		{
			componentAudioSource->PostEvent(AUDIO::SFX::PLAYER::ABILITIES::FORCE_STOP);
			isForceActive = false;
		}

		breakForce = false;
	}

	if (input->GetKey(SDL_SCANCODE_E) == KeyState::IDLE)
	{
		
		componentAnimation->SetParameter("IsStoppingForce", true);
		componentAnimation->SetParameter("IsStartingForce", false);
	}

	if (gameObjectAttached)
	{
		
		if (!isForceActive)
		{
			componentAudioSource->PostEvent(AUDIO::SFX::PLAYER::ABILITIES::FORCE_USE);
			isForceActive = true;
		}
		ComponentRigidBody* hittedRigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();
		ComponentTransform* hittedTransform = gameObjectAttached->GetComponent<ComponentTransform>();

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

		nextPosition.y = hittedTransform->GetGlobalPosition().y;

		float3 currentDistance = hittedTransform->GetGlobalPosition() - nextPosition;
		
		if (std::abs(currentDistance.x) > 1.5f && std::abs(currentDistance.z) > 1.5f && currentTimeForce < 14.5f)
		{
			breakForce = true;
			currentTimeForce = 10;
			return;
		} 

		float2 mouseMotion = input->GetMouseMotion();
		nextPosition.y = nextPosition.y -= mouseMotion.y * 0.2 * deltaTime;


		// Get next rotation of game object
		Quat targetRotation =
			Quat::RotateFromTo(hittedTransform->GetGlobalForward(),
				(transform->GetGlobalPosition() - hittedTransform->GetGlobalPosition()).Normalized());

		
		btRigidBody* rigidBody = hittedRigidBody->GetRigidBody();

		// Set position
		hittedRigidBody->SetPositionTarget(nextPosition);

		// Set rotation
		hittedRigidBody->SetRotationTarget(targetRotation);


		currentTimeForce -= deltaTime;
	}
	else if (currentTimeForce < maxTimeForce)
	{
		currentTimeForce = std::min(maxTimeForce, currentTimeForce + (deltaTime * 4));
	}
}