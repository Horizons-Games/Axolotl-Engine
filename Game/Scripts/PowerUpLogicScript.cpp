#include "PowerUpLogicScript.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/PowerUpsManagerScript.h"

REGISTERCLASS(PowerUpLogicScript);

#define SPAWN_LIFE 10.f

PowerUpLogicScript::PowerUpLogicScript() : Script()
{
	REGISTER_FIELD(powerUpsManager, GameObject*)
}

void PowerUpLogicScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	ownerRb = owner->GetComponent<ComponentRigidBody>();
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
	}
}

// Once requested, a given powerup will spawn in the given position
void PowerUpLogicScript::ActivatePowerUp(const float3& position)
{
	srand(static_cast<unsigned int>(time(0)));
	type = PowerUpType(rand() % 4);
	timer = 0.f;

	ownerTransform->SetPosition(position);
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

	PowerUpsManagerScript* playerManagerScript = powerUpsManager->GetComponent<PowerUpsManagerScript>();
	if (playerManagerScript->SavePowerUp(type))
	{
		DisablePowerUp();
	}
}

void PowerUpLogicScript::DisablePowerUp() const
{
	float3 position = ownerTransform->GetPosition();
	ownerTransform->SetPosition(float3(position.x, position.y - 2000, position.z));
	ownerTransform->UpdateTransformMatrices();

	ownerRb->UpdateRigidBody();

	owner->Disable();
}
