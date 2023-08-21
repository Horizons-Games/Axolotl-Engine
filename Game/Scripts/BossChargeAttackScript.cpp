#include "StdAfx.h"
#include "BossChargeAttackScript.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidbody.h"

#include "../Scripts/EnemyClass.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/BossChargeRockScript.h"

REGISTERCLASS(BossChargeAttackScript);

BossChargeAttackScript::BossChargeAttackScript() : Script(), chargeThroughPosition(nullptr), prepareChargeTime(0.0f),
	chargeCooldown(0.0f), transform(nullptr), rigidBody(nullptr), chargeState(ChargeState::NONE),
	chargeHitPlayer(false), bounceBackForce(5.0f), prepareChargeMaxTime(2.0f), chargeMaxCooldown(5.0f),
	attackStunTime(2.0f), chargeDamage(20.0f), rockPrefab(nullptr), spawningRockChance(5.0f), rockSpawningHeight(7.0f)
{
	REGISTER_FIELD(bounceBackForce, float);
	REGISTER_FIELD(prepareChargeMaxTime, float);
	REGISTER_FIELD(chargeMaxCooldown, float);
	REGISTER_FIELD(attackStunTime, float);
	REGISTER_FIELD(chargeDamage, float);

	REGISTER_FIELD(spawningRockChance, float);
	REGISTER_FIELD(rockSpawningHeight, float);

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

	else if (chargeState == ChargeState::NONE)
	{
		chargeCooldown -= deltaTime;
	}

	if (chargeState == ChargeState::CHARGING)
	{
		// Spawn rocks randomly over the charged zone
		int randomActivation = rand() % 100;

		if (randomActivation < spawningRockChance)
		{
			SpawnRock(float3(transform->GetGlobalPosition().x,
								transform->GetGlobalPosition().y + rockSpawningHeight,
								transform->GetGlobalPosition().z));
		}
	}
}


void BossChargeAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	// If the boss charges towards a wall or a rock, stop the charge and trigger the bounce back
	if ((other->GetOwner()->CompareTag("Wall") || other->GetOwner()->CompareTag("Rock"))
		&& chargeState == ChargeState::CHARGING)
	{
		chargeState = ChargeState::BOUNCING_WALL;

		WallHitAfterCharge();
		MakeRocksFall();
	}

	// If the boss is charging and hits the player, damage the player
	else if (other->GetOwner()->CompareTag("Player") && !chargeHitPlayer && chargeState == ChargeState::CHARGING)
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(chargeDamage);
		chargeHitPlayer = true;
	}

	// If the boss is bouncing and hits the floor, stop the bounce and stop the boss' movement (he'll be stunned)
	else if (other->GetOwner()->CompareTag("Floor") && chargeState == ChargeState::BOUNCING_WALL)
	{
		chargeState = ChargeState::NONE;

		rigidBody->SetIsKinematic(true);
		rigidBody->SetUpMobility();
	}
}


void BossChargeAttackScript::TriggerChargeAttack(GameObject* target)
{
	LOG_VERBOSE("Begin the charge attack");

	rigidBody->SetIsKinematic(false);
	rigidBody->SetUpMobility();

	rocksSpawned.clear();

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
	return chargeCooldown <= 0.0f && chargeState == ChargeState::NONE;
}

void BossChargeAttackScript::SpawnRock(float3 spawnPosition)
{
	LOG_DEBUG("Spawn rock at {}, {}, {}", spawnPosition.x, spawnPosition.y, spawnPosition.z);

	GameObject* newRock = App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject("Rock Copy", rockPrefab, rockPrefab->GetParent());

	ComponentTransform* newRockTransform = newRock->GetComponent<ComponentTransform>();
	newRockTransform->SetGlobalPosition(spawnPosition);
	newRockTransform->RecalculateLocalMatrix();

	newRock->Enable();
	ComponentRigidBody* newRockRigidBody = newRock->GetComponent<ComponentRigidBody>();
	newRockRigidBody->SetDefaultPosition();
	newRockRigidBody->Enable();

	rocksSpawned.push_back(newRock);
}

void BossChargeAttackScript::MakeRocksFall() const
{
	for (GameObject* spawnedRock : rocksSpawned)
	{
		if (!spawnedRock->IsEnabled())
		{
			continue;
		}

		ComponentRigidBody* rockRigidBody = spawnedRock->GetComponent<ComponentRigidBody>();
		rockRigidBody->SetIsTrigger(false);
		rockRigidBody->SetIsKinematic(false);
		rockRigidBody->SetUpMobility();

		spawnedRock->GetComponent<BossChargeRockScript>()->SetRockState(RockStates::FALLING);

		// This will need any kind of warning for the player in the future
		// Maybe a particle in the floor that shows where the rock is going to land
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