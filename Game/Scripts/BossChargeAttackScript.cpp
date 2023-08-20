#include "StdAfx.h"
#include "BossChargeAttackScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidbody.h"

#include "../Scripts/EnemyClass.h"
#include "../Scripts/HealthSystem.h"

#include "Physics/Physics.h"

REGISTERCLASS(BossChargeAttackScript);

BossChargeAttackScript::BossChargeAttackScript() : Script(), chargeThroughPosition(nullptr), prepareChargeTime(0.0f),
	chargeCooldown(0.0f), transform(nullptr), rigidBody(nullptr), chargeState(ChargeState::NOTHING),
	chargeHitPlayer(false), bounceBackForce(5.0f), prepareChargeMaxTime(2.0f), chargeMaxCooldown(5.0f),
	attackStunTime(2.0f), chargeDamage(20.0f), rockPrefab(nullptr)
{
	REGISTER_FIELD(bounceBackForce, float);
	REGISTER_FIELD(prepareChargeMaxTime, float);
	REGISTER_FIELD(chargeMaxCooldown, float);
	REGISTER_FIELD(attackStunTime, float);
	REGISTER_FIELD(chargeDamage, float);

	REGISTER_FIELD(rockPrefab, GameObject*);
}

void BossChargeAttackScript::Start()
{
	prepareChargeTime = prepareChargeMaxTime;

	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
}

void BossChargeAttackScript::Update(float deltaTime)
{
	// If the charge attack is triggered, prepare to charge
	if (chargeState == ChargeState::PREPARING_CHARGE)
	{
		PrepareCharge();

		prepareChargeTime -= deltaTime;
		if (prepareChargeTime <= 0.0f)
		{
			PerformChargeAttack();
		}
	}

	else if (chargeState == ChargeState::NOTHING)
	{
		chargeCooldown -= deltaTime;
	}

	if (chargeState == ChargeState::CHARGING)
	{
		// Spawn rocks randomly over the charged zone
		int randomActivation = rand() % 100;

		LOG_DEBUG("Random number {}", randomActivation);

		if (randomActivation < 5)
		{
			SpawnRock(float3(transform->GetGlobalPosition().x,
								transform->GetGlobalPosition().y + 20.0f,
								transform->GetGlobalPosition().z));
		}
	}
}


void BossChargeAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if ((other->GetOwner()->CompareTag("Wall") || other->GetOwner()->CompareTag("Rock"))
		&& chargeState == ChargeState::CHARGING)
	{
		chargeState = ChargeState::BOUNCING_WALL;

		WallHitAfterCharge();
		MakeRocksFall();
	}

	else if (!other->GetOwner()->CompareTag("Player") && !other->GetOwner()->CompareTag("Enemy") &&
		!other->GetOwner()->CompareTag("Wall") && !other->IsTrigger() && chargeState == ChargeState::BOUNCING_WALL)
	{
		chargeState = ChargeState::NOTHING;

		rigidBody->SetIsKinematic(true);
		rigidBody->SetUpMobility();
	}

	if (other->GetOwner()->CompareTag("Player") && !chargeHitPlayer)
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(chargeDamage);
		chargeHitPlayer = true;
	}
}


void BossChargeAttackScript::TriggerChargeAttack(GameObject* target)
{
	LOG_VERBOSE("Begin the charge attack");

	rigidBody->SetIsKinematic(false);
	rigidBody->SetUpMobility();

	chargeState = ChargeState::PREPARING_CHARGE;
	chargeThroughPosition = target->GetComponent<ComponentTransform>();
	chargeCooldown = chargeMaxCooldown;
	chargeHitPlayer = false;
}

void BossChargeAttackScript::PrepareCharge() const
{
	RotateToTarget(chargeThroughPosition);
	rigidBody->SetKpTorque(5.0f);

	float3 forward = transform->GetGlobalForward();
	forward.Normalize();

	rigidBody->SetKpForce(0.5f);
	rigidBody->SetPositionTarget(float3(forward.x * -5.0f + transform->GetGlobalPosition().x,
										transform->GetGlobalPosition().y,
										forward.z * -5.0f + transform->GetGlobalPosition().z));
}

void BossChargeAttackScript::PerformChargeAttack()
{
	LOG_VERBOSE("CHAAAAARGE!!!");

	float3 forward = transform->GetGlobalForward();
	forward.Normalize();

	rigidBody->SetKpForce(0.5f);
	rigidBody->SetPositionTarget(float3(forward.x * 50.0f,
										transform->GetGlobalPosition().y,
										forward.z * 50.0f));

	prepareChargeTime = prepareChargeMaxTime;
	chargeState = ChargeState::CHARGING;
}

void BossChargeAttackScript::WallHitAfterCharge() const
{
	// Same movement as when the player perfoms a jump attack, a small bounce backwards
	btRigidBody* enemybtRigidbody = rigidBody->GetRigidBody();
	rigidBody->DisablePositionController();
	rigidBody->DisableRotationController();
	enemybtRigidbody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	enemybtRigidbody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));

	float3 nextPosition = transform->GetGlobalPosition() - transform->GetGlobalPosition();
	nextPosition.Normalize();
	nextPosition += float3(0, 1.0f, 0);
	nextPosition *= bounceBackForce;

	btVector3 newVelocity(nextPosition.x, nextPosition.y, nextPosition.z);
	enemybtRigidbody->setLinearVelocity(newVelocity);

	EnemyClass* enemyScript = owner->GetComponent<EnemyClass>();
	enemyScript->SetStunnedTime(attackStunTime);
}

bool BossChargeAttackScript::CanPerformChargeAttack() const
{
	return chargeCooldown <= 0.0f;
}

void BossChargeAttackScript::SpawnRock(float3 spawnPosition) const
{
	LOG_DEBUG("Spawn rock at {}, {}, {}", spawnPosition.x, spawnPosition.y, spawnPosition.z);

	// Create rock
}

void BossChargeAttackScript::MakeRocksFall() const
{
	for (GameObject* spawnedRock : rocksSpawned)
	{
		if (!spawnedRock->IsEnabled())
		{
			continue;
		}

		// Set trigger to true
		// Set kinematic to false
	}
}

void BossChargeAttackScript::RotateToTarget(ComponentTransform* target) const
{
	Quat errorRotation =
		Quat::RotateFromTo(transform->GetGlobalForward().Normalized(),
			(target->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized());

#ifdef DEBUG
	dd::arrow(transform->GetGlobalPosition(),
		transform->GetGlobalPosition() + transform->GetGlobalForward() * 5.0f, dd::colors::Yellow, 1.0f);
	dd::arrow(transform->GetGlobalPosition(), target->GetGlobalPosition(), dd::colors::Green, 1.0f);
#endif // DEBUG

	rigidBody->SetRotationTarget(errorRotation);
}