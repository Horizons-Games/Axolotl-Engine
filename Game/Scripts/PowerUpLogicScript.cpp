#include "StdAfx.h"
#include "PowerUpLogicScript.h"

#include "Application.h"
#include "ModulePlayer.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpsManagerScript.h"

#include "debugdraw.h"
#include "Math/Quat.h"

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

	for (GameObject* child : owner->GetChildren())
	{
		if (child->CompareTag("HEAL"))
		{
			healPowerUp = child;
			healPowerUp->Disable();
		}
		else if (child->CompareTag("SHIELD"))
		{
			shieldPowerUp = child;
			shieldPowerUp->Disable();
		}
		else if (child->CompareTag("SPEED"))
		{
			speedPowerUp = child;
			speedPowerUp->Disable();
		}
		else if (child->CompareTag("DMG"))
		{
			dmgPowerUp = child;
			dmgPowerUp->Disable();
		}
	}

	DisablePowerUp();
}

void PowerUpLogicScript::Update(float deltaTime)
{
	if (owner->IsEnabled())
	{
		RotateMesh(deltaTime);
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
				btRigidBody* btRigidbody = ownerRb->GetRigidBody();
				btRigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
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

	ownerTransform->SetLocalPosition(float3::zero);
	ownerTransform->SetGlobalPosition(newParent->GetComponent<ComponentTransform>()->GetGlobalPosition());
	ownerTransform->RecalculateLocalMatrix();

	ownerRb->UpdateRigidBody();

	owner->Enable();

	if (type == PowerUpType::HEAL)
	{
		healPowerUp->Enable();
	}
	else if (type == PowerUpType::DEFENSE)
	{
		shieldPowerUp->Enable();
	}
	else if (type == PowerUpType::SPEED)
	{
		speedPowerUp->Enable();
	}
	else if (type == PowerUpType::ATTACK)
	{
		dmgPowerUp->Enable();
	}
}

void PowerUpLogicScript::RotateMesh(float deltatime)
{
	Quat newRotation = ownerTransform->GetGlobalRotation();
	newRotation.y = 2 * deltatime;
	ownerRb->SetRotationTarget(newRotation);
}

void PowerUpLogicScript::OnCollisionEnter(ComponentRigidBody* other)
{
	GameObject* hittedGameObject = other->GetOwner();
	if (!hittedGameObject->CompareTag("Player"))
	{
		return;
	}

	powerUpManagerScript->SavePowerUp(type);
	DisablePowerUp();
}

void PowerUpLogicScript::DisablePowerUp() const
{
	float3 position = ownerTransform->GetGlobalPosition();
	ownerTransform->SetGlobalPosition(float3(position.x, position.y - 200, position.z));
	ownerTransform->UpdateTransformMatrices();

	ownerRb->UpdateRigidBody();

	if (type == PowerUpType::HEAL)
	{
		healPowerUp->Disable();
	}
	else if (type == PowerUpType::DEFENSE)
	{
		shieldPowerUp->Disable();
	}
	else if (type == PowerUpType::SPEED)
	{
		speedPowerUp->Disable();
	}
	else if (type == PowerUpType::ATTACK)
	{
		dmgPowerUp->Disable();
	}

	owner->Disable();
}
