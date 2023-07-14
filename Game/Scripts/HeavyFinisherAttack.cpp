#include "HeavyFinisherAttack.h"

#include "Application.h"

#include "ModuleInput.h"

#include "Physics/Physics.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"

#include "../Scripts/HealthSystem.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include <set>

#include "AxoLog.h"

REGISTERCLASS(HeavyFinisherAttack);

HeavyFinisherAttack::HeavyFinisherAttack() : Script(), audioSource(nullptr), transform(nullptr), rigidBody(nullptr),
gameObject(nullptr), target(nullptr), isActivated(false), isReturningToOwner(false), attackOwner(nullptr), 
returnToPlayer(false), rotateWhileAttacking(true), damage(10.0f), speed(1.0f), hitDistance(1.0f)
{
	REGISTER_FIELD(returnToPlayer, bool);
	REGISTER_FIELD(rotateWhileAttacking, bool);
	REGISTER_FIELD(damage, float);
	REGISTER_FIELD(speed, float);
	REGISTER_FIELD(hitDistance, float);
}

void HeavyFinisherAttack::Start()
{
	gameObject = owner->GetComponent<GameObject>();
	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	audioSource = owner->GetComponent<ComponentAudioSource>();


	gameObject->Disable();
}

void HeavyFinisherAttack::Update(float deltaTime)
{
	if (isActivated)
	{
		float3 currentPos = transform->GetGlobalPosition();
		float3 enemyPos = target->GetGlobalPosition();

		float3 vecTowardsEnemy = (enemyPos - currentPos).Normalized();

		transform->SetPosition(currentPos + vecTowardsEnemy * speed * deltaTime);

		if (currentPos.Distance(enemyPos) < hitDistance)
		{
			if (!isReturningToOwner)
			{
				target->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(damage);

				bool hasAnotherTarget = SeekNextEnemy();

				if (!hasAnotherTarget && returnToPlayer)
				{
					target = attackOwner;
					isReturningToOwner = true;
				}
			}
			else
			{
				target = nullptr;
				attackOwner = nullptr;

				isActivated = false;
				isReturningToOwner = false;

				enemiesAlreadyHit.clear();
				enemiesInTheArea.clear();

				gameObject->Disable();
			}
		}
	}
}

void HeavyFinisherAttack::PerformHeavyFinisher(ComponentTransform* target, ComponentTransform* attackOwner)
{
	this->target = target;
	this->attackOwner = attackOwner;

	gameObject->Enable();

	isActivated = true;
}

bool HeavyFinisherAttack::SeekNextEnemy() 
{
	enemiesAlreadyHit.push_back(target);
	target = nullptr;

	if (enemiesInTheArea.empty())
	{
		return false;
	}

	float3 currentPos = transform->GetGlobalPosition();

	for (ComponentTransform* enemy : enemiesInTheArea)
	{
		if (std::find(enemiesAlreadyHit.begin(), enemiesAlreadyHit.end(), enemy) != enemiesAlreadyHit.end())
			continue;

		if (target == nullptr)
		{
			target = enemy;
		}
		else if (currentPos.Distance(enemy->GetGlobalPosition()) < currentPos.Distance(target->GetGlobalPosition()))
		{
			target = enemy;
		}
	}

	if (target == nullptr)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void HeavyFinisherAttack::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Enemy")
	{
		enemiesInTheArea.push_back(other->GetOwner()->GetComponent<ComponentTransform>());
	}
}

void HeavyFinisherAttack::OnCollisionExit(ComponentRigidBody* other)
{
	enemiesInTheArea.erase(
		std::remove_if(
			std::begin(enemiesInTheArea), std::end(enemiesInTheArea), [other](const ComponentTransform* transform)
			{
				return transform == other->GetOwner()->GetComponent<ComponentTransform>();
			}
		),
		std::end(enemiesInTheArea));
}