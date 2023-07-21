#include "StdAfx.h"
#include "PowerUpLogicScript.h"

#include "Application.h"
#include "ModulePlayer.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpsManagerScript.h"

#include "debugdraw.h"

REGISTERCLASS(PowerUpLogicScript);

#define SPAWN_LIFE 10.f

PowerUpLogicScript::PowerUpLogicScript() : Script(), isSeeking(false), timer(0.f)
{
	REGISTER_FIELD(powerUpsManager, GameObject*)
}

void PowerUpLogicScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	ownerRb = owner->GetComponent<ComponentRigidBody>();
	powerUpManagerScript = powerUpsManager->GetComponent<PowerUpsManagerScript>();

	GameObject* player = App->GetModule<ModulePlayer>()->GetPlayer();
	playerTransform = player->GetComponent<ComponentTransform>();
	DisablePowerUp();
}

void PowerUpLogicScript::Update(float deltaTime)
{
	if (owner->IsEnabled())
	{
		timer += deltaTime;

		if (timer >= SPAWN_LIFE)
		{
			DisablePowerUp();
		}
		float radius = powerUpManagerScript->GetRadiusSeeking();
		if (!isSeeking && ownerTransform->GetGlobalPosition().Equals(playerTransform->GetGlobalPosition(), radius)
			&& powerUpManagerScript->GetSavedPowerUpType() == PowerUpType::NONE)
		{
			isSeeking = true;
		}
		else if (isSeeking)
		{
			ownerRb->SetPositionTarget(playerTransform->GetGlobalPosition());
			Quat errorRotation =
				Quat::RotateFromTo(ownerTransform->GetGlobalForward().Normalized(),
					(playerTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition()).Normalized());
			ownerRb->SetRotationTarget(errorRotation.Normalized());
			if (powerUpManagerScript->GetSavedPowerUpType() != PowerUpType::NONE)
			{
				btRigidBody* btRb = ownerRb->GetRigidBody();
				btRb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
				isSeeking = false;
			}
		}
#ifdef DEBUG
		if (powerUpManagerScript->GetDebugDraw())
		{
			dd::sphere(ownerTransform->GetGlobalPosition(), dd::colors::Peru, radius);
		}
#endif // DEBUG
	}
}

// Once requested, a given powerup will spawn in the given position
void PowerUpLogicScript::ActivatePowerUp(GameObject* newParent)
{
	srand(static_cast<unsigned int>(time(0)));
	type = PowerUpType(rand() % 4 + 1);
	timer = 0.f;

	owner->SetParent(newParent);
	ownerTransform->SetLocalPosition(float3::zero);
	ownerTransform->UpdateTransformMatrices();

	ownerRb->UpdateRigidBody();

	owner->Enable();
}

void PowerUpLogicScript::OnCollisionEnter(ComponentRigidBody* other)
{
	GameObject* hittedGameObject = other->GetOwner();
	if (!hittedGameObject->CompareTag("Player"))
	{
		return;
	}

	if (powerUpManagerScript->SavePowerUp(type))
	{
		DisablePowerUp();
	}
}

void PowerUpLogicScript::DisablePowerUp() const
{
	float3 position = ownerTransform->GetGlobalPosition();
	ownerTransform->SetGlobalPosition(float3(position.x, position.y - 200, position.z));
	ownerTransform->UpdateTransformMatrices();

	ownerRb->UpdateRigidBody();

	owner->Disable();
}
