#include "PowerUpScript.h"

#include "Application.h"
#include "ModulePlayer.h"

#include "Physics/Physics.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "HealthSystem.h"
#include "PlayerMoveScript.h"
#include "BixAttackScript.h"

#include "GameObject/GameObject.h"

#include <set>

REGISTERCLASS(PowerUpScript);

#define HEALED_INCREASE 10.f
#define DEFENSE_INCREASE 10.f
#define ATTACK_INCREASE 10.f
#define SPEED_INCREASE 60.f

PowerUpScript::PowerUpScript() : Script()
{
}

void PowerUpScript::Start()
{
	ownerTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	ownerRb = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
}

void PowerUpScript::Update(float deltaTime)
{
}

void PowerUpScript::ActivatePowerUp(const float3& position)
{
	type = PowerUpType(rand() % 4);
	ownerTransform->SetPosition(float3(position.x, position.y - 200, position.z));
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
	DeactivatePowerUp();

	std::vector<ComponentScript*> gameObjectScripts =
		go->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);
	if (type == PowerUpType::HEAL)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
			{
				HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
				healthScript->HealLife(HEALED_INCREASE);
				return;
			}
		}
	}
	else if (type == PowerUpType::DEFENSE)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
			{
				HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
				healthScript->IncreaseDefense(DEFENSE_INCREASE);
				return;
			}
		}
	}
	else if (type == PowerUpType::ATTACK)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "BixAttackScript")
			{
				BixAttackScript* attackScript = static_cast<BixAttackScript*>(gameObjectScripts[i]->GetScript());
				attackScript->IncreaseAttack(ATTACK_INCREASE);
				return;
			}
		}
	}
	else if (type == PowerUpType::SPEED)
	{
		for (int i = 0; i < gameObjectScripts.size(); ++i)
		{
			if (gameObjectScripts[i]->GetConstructName() == "PlayerMoveScript")
			{
				PlayerMoveScript* moveScript = static_cast<PlayerMoveScript*>(gameObjectScripts[i]->GetScript());
				moveScript->IncreaseSpeed(SPEED_INCREASE);
				return;
			}
		}
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
