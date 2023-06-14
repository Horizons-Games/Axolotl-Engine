#include "PowerUpScript.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "PlayerManagerScript.h"

REGISTERCLASS(PowerUpScript);

#define SPAWN_LIFE 10.f

PowerUpScript::PowerUpScript() : Script()
{
}

void PowerUpScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	ownerRb = owner->GetComponent<ComponentRigidBody>();
	DeactivatePowerUp();
}

void PowerUpScript::Update(float deltaTime)
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
