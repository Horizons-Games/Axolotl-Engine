#include "StdAfx.h"
#include "HeavyFinisherAttack.h"

#include "Application.h"

#include "Physics/Physics.h"

#include "ModuleScene.h"
#include "ModulePhysics.h"
#include "Scene/Scene.h"

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
mesh(nullptr), target(nullptr), isActivated(false), isReturningToOwner(false), attackOwner(nullptr),
returnToPlayer(false), rotateWhileAttacking(true), damage(10.0f), speed(3.0f), hitDistance(1.0f), player(nullptr),
playerTransform(nullptr), loadedScene(nullptr)
{
	REGISTER_FIELD(returnToPlayer, bool);
	REGISTER_FIELD(rotateWhileAttacking, bool);
	REGISTER_FIELD(damage, float);
	REGISTER_FIELD(speed, float);
	REGISTER_FIELD(hitDistance, float);
	REGISTER_FIELD(mesh, GameObject*);
	REGISTER_FIELD(player, GameObject*);
}

void HeavyFinisherAttack::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	audioSource = owner->GetComponent<ComponentAudioSource>();

	playerTransform = player->GetComponent<ComponentTransform>();
	
	mesh->Disable();
	rigidBody->Disable();

	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
}

void HeavyFinisherAttack::Update(float deltaTime)
{
	if (!isActivated)
	{
		return;
	}


	float3 currentPos = transform->GetGlobalPosition();
	float3 enemyPos = target->GetGlobalPosition();

	float3 vecTowardsEnemy = (enemyPos - currentPos).Normalized();

	transform->SetPosition(currentPos + vecTowardsEnemy * speed * deltaTime);
	transform->UpdateTransformMatrices();

	if (currentPos.Distance(enemyPos) < hitDistance)
	{
		if (!isReturningToOwner)
		{
			target->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(damage);
			audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);

			bool hasAnotherTarget = SeekNextEnemy();

			if (!hasAnotherTarget && returnToPlayer)
			{
				target = attackOwner;
				isReturningToOwner = true;
			}
			else if (!hasAnotherTarget && !returnToPlayer)
			{
				isReturningToOwner = true;
				target = attackOwner;
				mesh->Disable();
				transform->SetPosition(playerTransform->GetGlobalPosition());
				transform->UpdateTransformMatrices();
			}
		}
		else
		{
			owner->SetParent(attackOwner->GetOwner());
			target = nullptr;
			attackOwner = nullptr;

			isActivated = false;
			isReturningToOwner = false;

			enemiesAlreadyHit.clear();
			enemiesInTheArea.clear();

			mesh->Disable();
		}
	}
}

void HeavyFinisherAttack::PerformHeavyFinisher(ComponentTransform* target, ComponentTransform* attackOwner)
{
	this->target = target;
	this->attackOwner = attackOwner;

	mesh->Enable();
	rigidBody->Disable();

	owner->SetParent(loadedScene->GetRoot());

	isActivated = true;
}

bool HeavyFinisherAttack::SeekNextEnemy()
{
	enemiesAlreadyHit.push_back(target);
	target = nullptr;

	rigidBody->Enable();
	rigidBody->UpdateRigidBody();
	ModulePhysics* physics = App->GetModule<ModulePhysics>();

	std::vector<ComponentRigidBody*> collisions;

	physics->GetCollisions(rigidBody, collisions, "Enemy");

	if (collisions.empty())
	{
		return false;
	}

	float3 currentPos = transform->GetGlobalPosition();

	for (ComponentRigidBody* enemy : collisions)
	{
		if (std::find(enemiesAlreadyHit.begin(), enemiesAlreadyHit.end(),
			enemy->GetOwner()->GetComponent<ComponentTransform>()) != enemiesAlreadyHit.end())
		{
			continue;
		}

		if (target == nullptr)
		{
			target = enemy->GetOwner()->GetComponent<ComponentTransform>();
		}
		else if (currentPos.Distance(enemy->GetOwner()->GetComponent<ComponentTransform>()->GetGlobalPosition()) 
			< currentPos.Distance(target->GetGlobalPosition()))
		{
			target = enemy->GetOwner()->GetComponent<ComponentTransform>();
		}
	}

	rigidBody->Disable();

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