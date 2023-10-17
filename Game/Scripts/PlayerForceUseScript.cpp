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

REGISTERCLASS(PlayerForceUseScript);

PlayerForceUseScript::PlayerForceUseScript() : Script(), 
	gameObjectAttached(nullptr), tag("Forceable"), distancePointGameObjectAttached(0.0f), 
	maxDistanceForce(20.0f), minDistanceForce(1.0f),
	componentAnimation(nullptr), componentAudioSource(nullptr), 
	playerManager(nullptr), gravity(0.0f), isForceActive(false)
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
			if (input->IsMouseWheelScrolled())
			{
				nextPosition.x -= input->GetMouseWheel().y * 10 * deltaTime;
				nextPosition.y += input->GetMouseWheel().y * 10 * deltaTime;
			}

			float2 mouseMotion = input->GetMouseMotion();
			nextPosition.x += mouseMotion.x * 0.1f * deltaTime;
			nextPosition.y -= mouseMotion.y * 0.1f * deltaTime;
		}
		else if (inputMethod == InputMethod::GAMEPAD)
		{
			// We divide by 1000 (*0.001) to move at aprox same speed as if we were using the mouse
			Sint16 horizontalMovement = int(input->GetLeftJoystickMovement().horizontalMovement * 0.001f);
			nextPosition.x += horizontalMovement * 0.2f * deltaTime;
			Sint16 verticalMovement = int(input->GetLeftJoystickMovement().verticalMovement * 0.001f);
			nextPosition.y -= verticalMovement * 0.2f * deltaTime;
		}
		
		distancePointGameObjectAttached = transform->GetGlobalPosition().Distance(nextPosition);
		if (distancePointGameObjectAttached > maxDistanceForce || distancePointGameObjectAttached < minDistanceForce)
		{
			hittedRigidBody->DisablePositionController();
			hittedRigidBody->DisableRotationController();
			hittedRigidBody->SetIsStatic(objectStaticness);
			hittedRigidBody->GetRigidBody()->setLinearVelocity({ 0.0f, 0.0f, 0.0f });

			gameObjectAttached = nullptr;
		}
		
		float3 desiredRotation = float3::zero;
		float3 vecForward = transform->GetGlobalForward().Normalized();
		float3 vecTowardsBox = (hittedTransform->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized();
		desiredRotation = vecForward + vecTowardsBox;
		desiredRotation.y = 0;
		desiredRotation = desiredRotation.Normalized();

		btTransform playerWorldTransform = rigidBody->GetRigidBody()->getWorldTransform();
		Quat lookAtRotation = Quat::LookAt(vecForward, desiredRotation, float3::unitY, float3::unitY);
		Quat targetRotation = lookAtRotation * transform->GetGlobalRotation();

		playerWorldTransform.setRotation({
			targetRotation.x,
			targetRotation.y,
			targetRotation.z,
			targetRotation.w
		});

		rigidBody->GetRigidBody()->setWorldTransform(playerWorldTransform);
		rigidBody->GetRigidBody()->getMotionState()->setWorldTransform(playerWorldTransform);
		
		hittedRigidBody->SetPositionTarget(nextPosition);
	}
	else if (isForceActive)
	{
		isForceActive = false;
		EnableAllInteractions();
		rigidBody->SetGravity({ 0.0f, gravity, 0.0f });
		componentAudioSource->PostEvent(AUDIO::SFX::PLAYER::ABILITIES::FORCE_STOP);
		componentAnimation->SetParameter("IsStoppingForce", true);
		componentAnimation->SetParameter("IsStartingForce", false);
	}
}

void PlayerForceUseScript::InitForce() 
{
	componentAudioSource->PostEvent(AUDIO::SFX::PLAYER::ABILITIES::FORCE_USE);
	componentAnimation->SetParameter("IsStartingForce", true);
	componentAnimation->SetParameter("IsStoppingForce", false);
	RaycastHit hit;
	btVector3 rigidBodyOrigin = rigidBody->GetRigidBodyOrigin();
	rigidBody->SetGravity({ 0.0f, 0.0f, 0.0f });
	float3 origin = float3(rigidBodyOrigin.getX(), rigidBodyOrigin.getY(), rigidBodyOrigin.getZ());
	int raytries = 0;

	while (gameObjectAttached == nullptr && raytries < 4)
	{
		Ray ray(origin + float3(0.f, 1.f * static_cast<float>(raytries), 0.f), transform->GetGlobalForward());
		LineSegment line(ray, 300);
		raytries++;
		isForceActive = true;

		if (Physics::RaycastToTag(line, hit, owner, tag))
		{
			gameObjectAttached = hit.gameObject;
			
			if (gameObjectAttached->GetChildren()[0]->HasComponent<ComponentParticleSystem>())
			{
				gameObjectAttached->GetChildren()[0]->GetComponent<ComponentParticleSystem>()->Stop();
			}

			distancePointGameObjectAttached = transform->GetGlobalPosition().Distance(hit.hitPoint);
			ComponentRigidBody* rigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();
			objectStaticness = rigidBody->IsStatic();
			rigidBody->SetIsStatic(false);

			rigidBody->SetKpForce(50.0f);
			rigidBody->SetKpTorque(50.0f);
		}
	}

	DisableAllInteractions();
}

void PlayerForceUseScript::FinishForce()
{
	isForceActive = false;
	EnableAllInteractions();
	rigidBody->SetGravity({ 0.0f, gravity, 0.0f });
	ComponentRigidBody* attachedRigidBody = gameObjectAttached->GetComponent<ComponentRigidBody>();

	attachedRigidBody->DisablePositionController();
	attachedRigidBody->DisableRotationController();
	attachedRigidBody->SetIsStatic(objectStaticness);
	attachedRigidBody->GetRigidBody()->setLinearVelocity({ 0.0f, 0.0f, 0.0f });
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
	return isForceActive;
}
