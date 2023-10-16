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
#include "Components/ComponentParticleSystem.h"
#include "DataModels/Camera/CameraGameObject.h"

#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"
#include "Auxiliar/Audio/AudioData.h"

#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/CameraControllerScript.h"
#include "../Scripts/PlayerMoveScript.h"
#include <AxoLog.h>

REGISTERCLASS(PlayerForceUseScript);

PlayerForceUseScript::PlayerForceUseScript() : Script(), 
	gameObjectAttached(nullptr), tag("Forceable"), distancePointGameObjectAttached(0.0f), 
	maxDistanceForce(10.0f), minDistanceForce(1.0f),
	componentAnimation(nullptr), componentAudioSource(nullptr), 
	playerManager(nullptr), gravity(0.0f)
{
	REGISTER_FIELD(maxDistanceForce, float);
	REGISTER_FIELD(minDistanceForce, float);
}

void PlayerForceUseScript::Start()
{
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();

	playerManager = owner->GetComponent<PlayerManagerScript>();
	moveScript = owner->GetComponent<PlayerMoveScript>();

	input = App->GetModule<ModuleInput>();
	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	gravity = rigidBody->GetGravity().getY();
}

void PlayerForceUseScript::Update(float deltaTime)
{
	if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN)
	{
		if (!gameObjectAttached)
		{
			InitForce();
		}
		else
		{
			FinishForce();
		}
	}

	if (gameObjectAttached)
	{
		ComponentRigidBody* hittedRigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();
		ComponentTransform* hittedTransform = gameObjectAttached->GetComponent<ComponentTransform>();

		float3 nextPosition = hittedTransform->GetGlobalPosition();

		InputMethod inputMethod = input->GetCurrentInputMethod();
		if (inputMethod == InputMethod::KEYBOARD)
		{
			float2 mouseMotion = input->GetMouseMotion();
			nextPosition.x += mouseMotion.x * 0.2f * deltaTime;
			nextPosition.y -= mouseMotion.y * 0.2f * deltaTime;
		}
		else if (inputMethod == InputMethod::GAMEPAD)
		{
			// We divide by 1000 (*0.001) to move at aprox same speed as if we where using the mouse
			Sint16 horizontalMovement = int(input->GetLeftJoystickMovement().horizontalMovement * 0.001f);
			nextPosition.x += horizontalMovement * 0.2f * deltaTime;
			Sint16 verticalMovement = int(input->GetLeftJoystickMovement().verticalMovement * 0.001f);
			nextPosition.y -= verticalMovement * 0.2f * deltaTime;
		}
		
		distancePointGameObjectAttached = transform->GetGlobalPosition().Distance(nextPosition);

		if (distancePointGameObjectAttached > maxDistanceForce)
		{
			gameObjectAttached = nullptr;
		}
		else if (distancePointGameObjectAttached < minDistanceForce)
		{
			distancePointGameObjectAttached = minDistanceForce;
		}
		
		// Set position
		hittedRigidBody->SetPositionTarget(nextPosition);
	}
}

void PlayerForceUseScript::InitForce() 
{
	DisableAllInteractions();
	componentAudioSource->PostEvent(AUDIO::SFX::PLAYER::ABILITIES::FORCE_USE);
	componentAnimation->SetParameter("IsStartingForce", true);
	componentAnimation->SetParameter("IsStoppingForce", false);
	RaycastHit hit;
	btVector3 rigidBodyOrigin = rigidBody->GetRigidBodyOrigin();
	rigidBody->SetGravity(btVector3(0.0, gravity, 0.0));
	float3 origin = float3(rigidBodyOrigin.getX(), rigidBodyOrigin.getY(), rigidBodyOrigin.getZ());
	int raytries = 0;

	while (gameObjectAttached == nullptr && raytries < 4)
	{
		Ray ray(origin + float3(0.f, 1.f * static_cast<float>(raytries), 0.f), transform->GetGlobalForward());
		LineSegment line(ray, 300);
		raytries++;

		if (Physics::RaycastToTag(line, hit, owner, tag))
		{
			gameObjectAttached = hit.gameObject;
			distancePointGameObjectAttached = transform->GetGlobalPosition().Distance(hit.hitPoint);
			ComponentRigidBody* rigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();
			objectStaticness = rigidBody->IsStatic();
			rigidBody->SetIsStatic(false);
			rigidBody->SetKpForce(50.0f);
			rigidBody->SetKpTorque(50.0f);
		}
	}

	if (!gameObjectAttached)
	{
		EnableAllInteractions();
		componentAudioSource->PostEvent(AUDIO::SFX::PLAYER::ABILITIES::FORCE_STOP);
		componentAnimation->SetParameter("IsStoppingForce", true);
		componentAnimation->SetParameter("IsStartingForce", false);
	}
}

void PlayerForceUseScript::FinishForce()
{
	EnableAllInteractions();
	rigidBody->SetGravity(btVector3(0.0, -150.0, 0.0));
	ComponentRigidBody* rigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();

	rigidBody->DisablePositionController();
	rigidBody->DisableRotationController();
	rigidBody->SetIsStatic(objectStaticness);
	rigidBody->GetRigidBody()->setLinearVelocity(btVector3(0.0f,0.0f,0.0f));
	gameObjectAttached = nullptr;

	componentAudioSource->PostEvent(AUDIO::SFX::PLAYER::ABILITIES::FORCE_STOP);
	componentAnimation->SetParameter("IsStoppingForce", true);
	componentAnimation->SetParameter("IsStartingForce", false);
}

void PlayerForceUseScript::DisableAllInteractions() const
{
	playerManager->SetPlayerState(PlayerActions::IDLE);
	playerManager->PausePlayer(true);
}

void PlayerForceUseScript::EnableAllInteractions() const
{
	playerManager->SetPlayerState(PlayerActions::IDLE);
	playerManager->PausePlayer(false);
}

bool PlayerForceUseScript::IsForceActive() const
{
	return gameObjectAttached != nullptr;
}
