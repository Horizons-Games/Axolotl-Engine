#include "StdAfx.h"
#include "BossChargeAttackScript.h"

#include "Application.h"
#include "Modules/ModuleRandom.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidbody.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/EnemyClass.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/BossChargeRockScript.h"
#include "../Scripts/FinalBossScript.h"

REGISTERCLASS(BossChargeAttackScript);

BossChargeAttackScript::BossChargeAttackScript() : Script(), chargeThroughPosition(nullptr), prepareChargeTime(0.0f),
	chargeCooldown(0.0f), transform(nullptr), rigidBody(nullptr), chargeState(ChargeState::NONE),
	chargeHitPlayer(false), bounceBackForce(5.0f), prepareChargeMaxTime(2.0f), chargeMaxCooldown(5.0f),
	attackStunTime(4.0f), chargeDamage(20.0f), rockPrefab(nullptr), spawningRockChance(5.0f), rockSpawningHeight(7.0f),
	isRockAttackVariant(false), animator(nullptr)
{
	REGISTER_FIELD(bounceBackForce, float);
	REGISTER_FIELD(prepareChargeMaxTime, float);
	REGISTER_FIELD(chargeMaxCooldown, float);
	REGISTER_FIELD(attackStunTime, float);
	REGISTER_FIELD(chargeDamage, float);

	REGISTER_FIELD(spawningRockChance, float);
	REGISTER_FIELD(rockSpawningHeight, float);

	REGISTER_FIELD(rockPrefab, GameObject*);

	REGISTER_FIELD(isRockAttackVariant, bool);
}

void BossChargeAttackScript::Start()
{
	prepareChargeTime = prepareChargeMaxTime;

	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	animator = owner->GetComponent<ComponentAnimation>();

	finalBossScript = owner->GetComponent<FinalBossScript>();
}

void BossChargeAttackScript::Update(float deltaTime)
{
	ManageChargeAttackStates(deltaTime);
}

void BossChargeAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (chargeState == ChargeState::PREPARING_CHARGE && other->GetOwner()->CompareTag("Wall"))
	{
		prepareChargeTime = 0.0f;
	}
	else if (chargeState == ChargeState::CHARGING &&
			(other->GetOwner()->CompareTag("Wall") || other->GetOwner()->CompareTag("Rock")))
	{
		chargeState = ChargeState::BOUNCING_WALL;
		animator->SetParameter("IsCharging", false);
		animator->SetParameter("IsChargingHitWall", true);

		WallHitAfterCharge();

		if (isRockAttackVariant)
		{
			MakeRocksFall();
		}
	}
	else if (chargeState == ChargeState::CHARGING && !chargeHitPlayer && other->GetOwner()->CompareTag("Player"))
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(chargeDamage);
		chargeHitPlayer = true;
	}
	else if (chargeState == ChargeState::BOUNCING_WALL && other->GetOwner()->CompareTag("Floor"))
	{
		/*rigidBody->SetIsKinematic(true);
		rigidBody->SetUpMobility();*/
	}
}

void BossChargeAttackScript::TriggerChargeAttack(ComponentTransform* targetPosition)
{
	LOG_INFO("The charge attack was triggered");
	finalBossScript->RemoveAgent();

	chargeState = ChargeState::PREPARING_CHARGE;
	animator->SetParameter("IsChargingHitWall", false);
	animator->SetParameter("IsPreparingChargeAttack", true);
	chargeCooldown = chargeMaxCooldown;

	chargeThroughPosition = targetPosition;
	chargeHitPlayer = false;
}

void BossChargeAttackScript::ManageChargeAttackStates(float deltaTime)
{
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
		rigidBody->SetIsTrigger(true);
		rigidBody->SetYAxisBlocked(true);
		rigidBody->SetUpMobility();
		if (isRockAttackVariant)
		{
			float spawnRockActualChange = App->GetModule<ModuleRandom>()->RandomNumberInRange(100.0f);

			if (spawnRockActualChange < spawningRockChance)
			{
				SpawnRock(float3(transform->GetGlobalPosition().x,
					transform->GetGlobalPosition().y + rockSpawningHeight,
					transform->GetGlobalPosition().z));
			}
		}

		/*
		// This else will manage the CHARGING behavior of the first miniboss, where,
		// instead of throwing rocks, it will leave in the floor a toxic trail
		else
		{

		}
		*/
	}

	if (chargeState == ChargeState::BOUNCING_WALL)
	{
		rigidBody->SetIsTrigger(false);
		rigidBody->SetYAxisBlocked(false);
		rigidBody->SetUpMobility();

		EnemyClass* enemyScript = owner->GetComponent<EnemyClass>();
		if (enemyScript->GetStunnedTime() <= 0.0f)
		{
			/*rigidBody->SetXRotationAxisBlocked(false);
			rigidBody->SetYRotationAxisBlocked(false);
			rigidBody->SetZRotationAxisBlocked(false);*/

			rocksSpawned.clear();

			chargeState = ChargeState::NONE;
		}
		else
		{
			enemyScript->SetStunnedTime(enemyScript->GetStunnedTime() - deltaTime);
		}
	}
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
	float3 forward = transform->GetGlobalForward();
	forward.Normalize();

	/*rigidBody->SetXRotationAxisBlocked(true);
	rigidBody->SetYRotationAxisBlocked(true);
	rigidBody->SetZRotationAxisBlocked(true);*/

	rigidBody->SetKpForce(0.5f);
	rigidBody->SetPositionTarget(float3(forward.x * 50.0f,
										transform->GetGlobalPosition().y,
										forward.z * 50.0f));

	prepareChargeTime = prepareChargeMaxTime;
	chargeState = ChargeState::CHARGING;
	animator->SetParameter("IsPreparingChargeAttack", false);
	animator->SetParameter("IsCharging", true);
}

void BossChargeAttackScript::WallHitAfterCharge() const
{
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

bool BossChargeAttackScript::IsAttacking() const
{
	return chargeState != ChargeState::NONE;
}

void BossChargeAttackScript::SpawnRock(const float3& spawnPosition)
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

	rigidBody->SetRotationTarget(errorRotation);
}