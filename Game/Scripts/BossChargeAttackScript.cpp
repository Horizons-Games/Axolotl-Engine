#include "StdAfx.h"
#include "BossChargeAttackScript.h"

#include "Application.h"
#include "Modules/ModuleRandom.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"
#include "Auxiliar/Audio/AudioData.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidbody.h"
#include "Components/ComponentBreakable.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentObstacle.h"
#include "Components/ComponentAudioSource.h"

#include "../Scripts/EnemyClass.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/BossChargeRockScript.h"
#include "../Scripts/FinalBossScript.h"
#include "../Scripts/BossWallChecker.h"

REGISTERCLASS(BossChargeAttackScript);

BossChargeAttackScript::BossChargeAttackScript() : Script(), chargeThroughPosition(nullptr), prepareChargeTime(0.0f),
	chargeCooldown(0.0f), transform(nullptr), rigidBody(nullptr), chargeState(ChargeState::NONE),
	chargeHitPlayer(false), bounceBackForce(5.0f), prepareChargeMaxTime(2.0f), chargeMaxCooldown(5.0f),
	attackStunTime(4.0f), chargeDamage(20.0f), rockPrefab(nullptr), spawningRockChance(5.0f), rockSpawningHeight(7.0f),
	isRockAttackVariant(false), animator(nullptr), chargeForce(1.25f), wallChecker(nullptr)
{
	REGISTER_FIELD(bounceBackForce, float);
	REGISTER_FIELD(prepareChargeMaxTime, float);
	REGISTER_FIELD(chargeMaxCooldown, float);
	REGISTER_FIELD(attackStunTime, float);
	REGISTER_FIELD(chargeForce, float);
	REGISTER_FIELD(chargeDamage, float);

	REGISTER_FIELD(spawningRockChance, float);
	REGISTER_FIELD(rockSpawningHeight, float);

	REGISTER_FIELD(rockPrefab, GameObject*);

	REGISTER_FIELD(isRockAttackVariant, bool);

	REGISTER_FIELD(wallChecker, BossWallChecker*);
}

void BossChargeAttackScript::Start()
{
	prepareChargeTime = prepareChargeMaxTime;
	chargeCooldown = chargeMaxCooldown;

	transform = owner->GetComponent<ComponentTransform>();
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	animator = owner->GetComponent<ComponentAnimation>();
	audioSource = owner->GetComponent<ComponentAudioSource>();

	finalBossScript = owner->GetComponent<FinalBossScript>();
}

void BossChargeAttackScript::Update(float deltaTime)
{
	if (!isPaused)
	{
		if (chargeState == ChargeState::CHARGING)
		{
			rigidBody->SetKpForce(chargeForce);
		}
		ManageChargeAttackStates(deltaTime);
	}
}

void BossChargeAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (chargeState == ChargeState::PREPARING_CHARGE && 
		(other->GetOwner()->CompareTag("Wall") || other->GetOwner()->CompareTag("Rock")))
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

		// VFX Here: The boss hit the wall after a charge attack
	}
	else if (chargeState == ChargeState::CHARGING && !chargeHitPlayer && other->GetOwner()->CompareTag("Player"))
	{
		other->GetOwner()->GetComponent<HealthSystem>()->TakeDamage(chargeDamage);
		chargeHitPlayer = true;

		audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::CHARGE_WALL_HIT);
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

	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::CHARGE_LOAD);

	// VFX Here: The boss started the charge attack (going backwards or yelling, whatever you want to add)
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
		if (isRockAttackVariant && !wallChecker->IsFacingNearWall())
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

	rigidBody->SetKpForce(chargeForce);
	rigidBody->SetPositionTarget(float3(forward.x * 50.0f,
										transform->GetGlobalPosition().y,
										forward.z * 50.0f));

	prepareChargeTime = prepareChargeMaxTime;
	chargeState = ChargeState::CHARGING;
	animator->SetParameter("IsPreparingChargeAttack", false);
	animator->SetParameter("IsCharging", true);

	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::CHARGE_ATTACK);
	// VFX Here: The boss started the charging forward
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

	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::CHARGE_WALL_HIT);
	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::CHARGE_WALL_STUNT);
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
	newRockTransform->SetGlobalRotation(
		float3(newRockTransform->GetGlobalRotation().x, 
			App->GetModule<ModuleRandom>()->RandomNumberInRange(360.f),
			newRockTransform->GetGlobalRotation().z));
	newRockTransform->RecalculateLocalMatrix();
	newRockTransform->UpdateTransformMatrices();

	ComponentRigidBody* newRockRigidBody = newRock->GetComponent<ComponentRigidBody>();
	newRockRigidBody->UpdateRigidBody();
	newRockRigidBody->Enable();

	ComponentBreakable* newRockBreakable = newRock->GetComponent<ComponentBreakable>();

	ComponentParticleSystem* newRockVFX = newRock->GetComponent<ComponentParticleSystem>();

	if (!newRock->GetChildren().empty())
	{
		newRock->GetChildren().front()->Enable();
	}

	rocksSpawned.push_back(newRock);

	audioSource->PostEvent(AUDIO::SFX::NPC::FINALBOSS::CHARGE_ROCKS);
}

void BossChargeAttackScript::MakeRocksFall() const
{
	for (GameObject* spawnedRock : rocksSpawned)
	{
		if (!spawnedRock->IsEnabled())
		{
			spawnedRock->GetComponent<BossChargeRockScript>()->DestroyRock();
			continue;
		}

		spawnedRock->GetComponent<ComponentObstacle>()->RemoveObstacle();

		ComponentRigidBody* rockRigidBody = spawnedRock->GetComponent<ComponentRigidBody>();
		rockRigidBody->SetIsTrigger(false);
		rockRigidBody->SetIsKinematic(false);
		rockRigidBody->SetUpMobility();

		spawnedRock->GetComponent<BossChargeRockScript>()->SetRockState(RockStates::FALLING);

		// VFX Here: Rock falling warning
	}
}

void BossChargeAttackScript::RotateToTarget(ComponentTransform* target) const
{
	Quat errorRotation =
		Quat::RotateFromTo(transform->GetGlobalForward().Normalized(),
			(target->GetGlobalPosition() - transform->GetGlobalPosition()).Normalized());

	rigidBody->SetRotationTarget(errorRotation);
}

void BossChargeAttackScript::SetIsPaused(bool isPaused)
{
	rigidBody->SetKpForce(0.f);
	this->isPaused = isPaused;

	if (!isPaused && chargeState == ChargeState::CHARGING)
	{
		rigidBody->SetKpForce(0.5f);
	}
}