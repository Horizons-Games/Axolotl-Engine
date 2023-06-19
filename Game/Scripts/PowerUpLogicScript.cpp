#include "PowerUpLogicScript.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "PlayerManagerScript.h"

REGISTERCLASS(PowerUpLogicScript);

#define SPAWN_LIFE 10.f

PowerUpLogicScript::PowerUpLogicScript() : Script()
{
}

void PowerUpLogicScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	ownerRb = owner->GetComponent<ComponentRigidBody>();
	DeactivatePowerUp();
}

void PowerUpLogicScript::Update(float deltaTime)
{
	if (owner->IsEnabled())
	{
		counter += deltaTime;
		if (counter >= SPAWN_LIFE)
		{
			DeactivatePowerUp();
		}
	}
}

// Once requested, a given powerup will spawn in the given position
void PowerUpLogicScript::ActivatePowerUp(const float3& position)
{
	type = PowerUpType(rand() % 4);
	counter = 0.f;

	ownerTransform->SetPosition(position);
	ownerTransform->UpdateTransformMatrices();

	ownerRb->UpdateRigidBody();

	owner->Enable();
}

void PowerUpLogicScript::OnCollisionEnter(ComponentRigidBody* other)
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

void PowerUpLogicScript::DeactivatePowerUp()
{
	float3 position = ownerTransform->GetPosition();
	ownerTransform->SetPosition(float3(position.x, position.y - 2000, position.z));
	ownerTransform->UpdateTransformMatrices();

	ownerRb->UpdateRigidBody();

	owner->Disable();
}
