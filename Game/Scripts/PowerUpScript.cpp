#include "PowerUpScript.h"

#include "Application.h"
#include "ModulePlayer.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "GameObject/GameObject.h"

#include "PlayerManagerScript.h"

#include "debugdraw.h"

REGISTERCLASS(PowerUpScript);

#define SPAWN_LIFE 10.f

PowerUpScript::PowerUpScript() : Script(), radius(1.f), isSeeking(false)
{
	REGISTER_FIELD(radius, float);
}

void PowerUpScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	ownerRb = owner->GetComponent<ComponentRigidBody>();
	GameObject* player = App->GetModule<ModulePlayer>()->GetPlayer();
	playerTransform = player->GetComponent<ComponentTransform>();
	playerManagerScript = player->GetComponent<PlayerManagerScript>();
	//DeactivatePowerUp();
}

void PowerUpScript::Update(float deltaTime)
{
	if (owner->IsEnabled())
	{
		counter += deltaTime;
		/*if (counter >= SPAWN_LIFE)
		{
			DeactivatePowerUp();
		}*/
		if (!isSeeking && ownerTransform->GetGlobalPosition().Equals(playerTransform->GetGlobalPosition(), radius) 
			&& playerManagerScript->GetPowerUpType() == PowerUpType::NONE)
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
			if (playerManagerScript->GetPowerUpType() != PowerUpType::NONE)
			{
				btRigidBody* btRb = ownerRb->GetRigidBody();
				btRb->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f)); // the power Up stills following 
																	  // the player at a low speed
				isSeeking = false;
			}
		}
#ifdef DEBUG
		dd::sphere(ownerTransform->GetGlobalPosition(), dd::colors::Yellow, radius);
#endif // DEBUG
	}
}

void PowerUpScript::ActivatePowerUp(const float3& position)
{
	type = PowerUpType(rand() % 4);
	counter = 0.f;
	ownerTransform->SetPosition(position);
	ownerTransform->UpdateTransformMatrices();
	ownerRb->UpdateRigidBody();
	owner->Enable();
}

void PowerUpScript::OnCollisionEnter(ComponentRigidBody* other)
{
	GameObject* go = other->GetOwner();
	if (!go->CompareTag("Player"))
	{
		return;
	}

	PlayerManagerScript* playerManagerScript = go->GetComponent<PlayerManagerScript>();
	if (playerManagerScript->SavePowerUp(type))
	{
		DeactivatePowerUp();
	}
}

void PowerUpScript::DeactivatePowerUp()
{
	float3 position = ownerTransform->GetPosition();
	ownerTransform->SetPosition(float3(position.x, position.y - 200, position.z));
	ownerTransform->UpdateTransformMatrices();
	ownerRb->UpdateRigidBody();
	owner->Disable();
}
