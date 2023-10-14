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
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerAttackScript.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include <set>

#include "AxoLog.h"

REGISTERCLASS(HeavyFinisherAttack);

HeavyFinisherAttack::HeavyFinisherAttack() : Script(), audioSource(nullptr), transform(nullptr), rigidBody(nullptr),
mesh(nullptr), target(nullptr), isActivated(false), isReturningToOwner(false), attackHasTarget(false), attackOwner(nullptr),
vfx(nullptr), returnToPlayer(false), rotateWhileAttacking(true), damage(10.0f), speed(12.0f), hitDistance(1.0f), 
rotationVelocity(50.0f), loadedScene(nullptr), physics(nullptr), maxEnemyHits(3.0f),
defaultThrowDistance(10.0f)
{
	REGISTER_FIELD(returnToPlayer, bool);
	REGISTER_FIELD(rotateWhileAttacking, bool);
	REGISTER_FIELD(damage, float);
	REGISTER_FIELD(speed, float);
	REGISTER_FIELD(hitDistance, float);
	REGISTER_FIELD(rotationVelocity, float);
	REGISTER_FIELD(maxEnemyHits, float);
	REGISTER_FIELD(mesh, GameObject*);
}

void HeavyFinisherAttack::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	audioSource = owner->GetComponent<ComponentAudioSource>();
	vfx = owner->GetComponent<ComponentParticleSystem>();
	playerAttackScript = owner->GetParent()->GetComponent<PlayerAttackScript>();

	mesh->Disable();
	rigidBody->Disable();

	physics = App->GetModule<ModulePhysics>();
	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
}

void HeavyFinisherAttack::Update(float deltaTime)
{
	if (!isActivated)
	{
		return;
	}

	if (attackHasTarget)
	{
		MoveToEnemy(deltaTime);
	}
	else
	{
		MoveForward(deltaTime);
	}
}

void HeavyFinisherAttack::MoveToEnemy(float deltaTime) 
{
	float3 currentPos = transform->GetGlobalPosition();
	float3 enemyPos = target->GetGlobalPosition();

	float3 vecTowardsEnemy = (enemyPos - currentPos).Normalized();

	transform->SetLocalPosition(currentPos + vecTowardsEnemy * speed * deltaTime); //Move attack
	if (rotateWhileAttacking)
	{
		transform->SetLocalRotation(Quat::RotateAxisAngle(float3::unitY, math::DegToRad(rotationVelocity * 10.0f * deltaTime))
			* transform->GetLocalRotation());
	}
	transform->UpdateTransformMatrices();

	if (currentPos.Distance(enemyPos) < hitDistance) //Attack hits target
	{
		if (!isReturningToOwner)
		{
			HitEnemy();

			if (enemiesAlreadyHit.size() >= maxEnemyHits)
			{
				if (!returnToPlayer)
				{
					ResetValues();
				}
				else
				{
					target = attackOwner;
					isReturningToOwner = true;
				}
			}
			else
			{
				SeekNextEnemy();
			}
		}
		else
		{
			ResetValues();
		}
	}
}

void HeavyFinisherAttack::MoveForward(float deltaTime)
{
	float3 currentPos = transform->GetGlobalPosition();

	float3 vecForward = (emptyAttackTargetPos - currentPos).Normalized();

	transform->SetLocalPosition(currentPos + vecForward * speed * deltaTime); //Move attack
	if (rotateWhileAttacking)
	{
		transform->SetLocalRotation(Quat::RotateAxisAngle(float3::unitY, math::DegToRad(rotationVelocity * 10.0f * deltaTime))
			* transform->GetLocalRotation());
	}
	transform->UpdateTransformMatrices();
	rigidBody->UpdateRigidBody();

	bool enemyHit = false;
	for (ComponentTransform* enemy : enemiesInTheArea)
	{
		if (currentPos.Distance(enemy->GetGlobalPosition()) < hitDistance)
		{
			target = enemy;
			HitEnemy();
			enemyHit = true;
			SeekNextEnemy();
			attackHasTarget = true;
			
			break;
		}
	}

	if (!enemyHit)
	{
		if (currentPos.Distance(emptyAttackTargetPos) < hitDistance) //Attack gets to target
		{
			target = attackOwner;
			isReturningToOwner = true;
			attackHasTarget = true;
			rigidBody->Disable();
			if (!returnToPlayer)
			{
				ResetValues();
			}
		}
	}
}

void HeavyFinisherAttack::HitEnemy()
{
	target->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(damage);
	if (playerAttackScript->IsMelee())
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_CLASH);
	}
	else
	{
		audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::ELECTRIC_SHOT_IMPACT); // Provisional sfx
	}
	vfx->SetPlayAtStart(true);
	vfx->Play();
	enemiesAlreadyHit.push_back(target);
}

void HeavyFinisherAttack::PerformHeavyFinisher(ComponentTransform* target, ComponentTransform* attackOwner)
{
	this->target = target;
	this->attackOwner = attackOwner;

	attackHasTarget = true;

	mesh->Enable();
	rigidBody->Disable();

	owner->SetParent(loadedScene->GetRoot());

	isReturningToOwner = false;
	isActivated = true;
}

void HeavyFinisherAttack::PerformEmptyHeavyFinisher(ComponentTransform* attackOwner)
{
	this->attackOwner = attackOwner;

	emptyAttackTargetPos = attackOwner->GetGlobalPosition() + attackOwner->GetGlobalForward().Normalized() * defaultThrowDistance;

	attackHasTarget = false;

	mesh->Enable();
	rigidBody->Enable();

	owner->SetParent(loadedScene->GetRoot());

	std::vector<ComponentRigidBody*> collisions;
	physics->GetCollisions(rigidBody, collisions, "Enemy"); //Get enemies inside the collider

	for (ComponentRigidBody* enemy : collisions)
	{
		enemiesInTheArea.push_back(enemy->GetOwner()->GetComponent<ComponentTransform>());
	}

	isReturningToOwner = false;
	isActivated = true;
}

void HeavyFinisherAttack::SeekNextEnemy()
{
	target = nullptr;

	rigidBody->Enable();
	rigidBody->UpdateRigidBody();

	std::vector<ComponentRigidBody*> collisions;
	physics->GetCollisions(rigidBody, collisions, "Enemy"); //Get enemies inside the collider

	float3 currentPos = transform->GetGlobalPosition();

	for (ComponentRigidBody* enemy : collisions)
	{
		ComponentTransform* enemyTransform = enemy->GetOwner()->GetComponent<ComponentTransform>();

		if (!enemy->GetOwner()->GetComponent<HealthSystem>()->EntityIsAlive() //If enemy is dead
			|| (std::any_of(enemiesAlreadyHit.begin(), enemiesAlreadyHit.end(), 
				[&enemyTransform](const ComponentTransform* other) { return other == enemyTransform; }))) //If enemy already hit
		{
			continue; //next enemy
		}

		if (target == nullptr) //First enemy found
		{
			target = enemyTransform;
		}
		else if (currentPos.Distance(enemyTransform->GetGlobalPosition())
			< currentPos.Distance(target->GetGlobalPosition())) //Is a closer enemy than the selected
		{
			target = enemyTransform;
		}
	}

	rigidBody->Disable();

	if (target == nullptr) //No enemies to hit found
	{
		target = attackOwner;
		isReturningToOwner = true;
		if (!returnToPlayer)
		{
			ResetValues();
		}
	}
}

void HeavyFinisherAttack::ResetValues()
{
	transform->SetLocalPosition(attackOwner->GetGlobalPosition());
	transform->UpdateTransformMatrices();
	owner->SetParent(attackOwner->GetOwner());
	target = nullptr;
	attackOwner = nullptr;

	isActivated = false;
	isReturningToOwner = false;
	attackHasTarget = false;

	enemiesAlreadyHit.clear();
	enemiesInTheArea.clear();

	vfx->Stop();
	mesh->Disable();
}

void HeavyFinisherAttack::OnCollisionEnter(ComponentRigidBody* other)
{
	if (attackHasTarget)
	{
		return;
	}

	if (other->GetOwner()->CompareTag("Enemy") && other->GetOwner()->IsEnabled())
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

bool HeavyFinisherAttack::IsAttacking()
{
	return isActivated;
}