#include "EnemyDroneScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/RangedFastAttackBehaviourScript.h"
#include "../Scripts/MeleeHeavyAttackBehaviourScript.h"
#include "../Scripts/HealthSystem.h"
#include "../Scripts/PlayerManagerScript.h"
#include "../Scripts/AIMovement.h"
#include "../Scripts/PathBehaviourScript.h"

#include "../Scripts/EnemyDeathScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(EnemyDroneScript);

EnemyDroneScript::EnemyDroneScript() : patrolScript(nullptr), seekScript(nullptr), fastAttackScript(nullptr),
droneState(DroneBehaviours::IDLE), ownerTransform(nullptr), attackDistance(3.0f), seekDistance(6.0f),
componentAnimation(nullptr), componentAudioSource(nullptr), heavyAttackScript(nullptr),
explosionGameObject(nullptr), playerManager(nullptr), aiMovement(nullptr), flinchAnimationOffset(false),
exclamationVFX(nullptr), enemyDetectionDuration(0.0f), enemyDetectionTime(0.0f), minStopTimeAfterSeek(0.0f),
minStopDurationAfterSeek(1.0f), pathScript(nullptr)
{
	// seekDistance should be greater than attackDistance, because first the drone seeks and then attacks
	REGISTER_FIELD(attackDistance, float);
	REGISTER_FIELD(seekDistance, float);

	REGISTER_FIELD(explosionGameObject, GameObject*);

	REGISTER_FIELD(exclamationVFX, ComponentParticleSystem*);
	REGISTER_FIELD(enemyDetectionDuration, float);

	REGISTER_FIELD(minStopDurationAfterSeek, float);
}

void EnemyDroneScript::Start()
{
	enemyType = EnemyTypes::DRONE;

	if (seekDistance < attackDistance)
	{
		seekDistance = attackDistance;
	}

	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();

	seekScript = owner->GetComponent<SeekBehaviourScript>();
	fastAttackScript = owner->GetComponent<RangedFastAttackBehaviourScript>();
	heavyAttackScript = explosionGameObject->GetComponent<MeleeHeavyAttackBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();
	aiMovement = owner->GetComponent<AIMovement>();

	if(owner->HasComponent<PathBehaviourScript>())
	{
		pathScript = owner->GetComponent<PathBehaviourScript>();
		droneState = DroneBehaviours::INPATH;
	}

	playerManager = seekTarget->GetComponent<PlayerManagerScript>();
	enemyDetectionTime = 0.0f;
}

void EnemyDroneScript::Update(float deltaTime)
{
	if (paralyzed)
	{
		return;
	}
	seekTarget = seekScript->GetTarget();
	seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();
	playerManager = seekTarget->GetComponent<PlayerManagerScript>();

	if (stunned && droneState != DroneBehaviours::READYTOEXPLODE && droneState != DroneBehaviours::EXPLOSIONATTACK)
	{
		if (timeStunned < 0)
		{
			timeStunned = 0;
			stunned = false;
		}
		else
		{
			timeStunned -= deltaTime;
			return;
		}
	}

	CheckState(deltaTime);

	UpdateBehaviour(deltaTime);
}

void EnemyDroneScript::CheckState(float deltaTime)
{
	if (droneState == DroneBehaviours::EXPLOSIONATTACK 
		|| droneState == DroneBehaviours::INPATH)
	{
		return;
	}

	if (droneState == DroneBehaviours::READYTOEXPLODE)
	{
		if (droneState != DroneBehaviours::EXPLOSIONATTACK && componentAnimation->GetActualStateName() != "Flinch"
			&& flinchAnimationOffset == true)
		{
			componentAnimation->SetParameter("IsSeeking", false);
			componentAnimation->SetParameter("IsAttacking", false);
			componentAnimation->SetParameter("IsStopToAttack", false);
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			heavyAttackScript->TriggerExplosion();

			aiMovement->SetMovementStatuses(true, true);

			droneState = DroneBehaviours::EXPLOSIONATTACK;
		}
		flinchAnimationOffset = true;
	}
	else if (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(), attackDistance))
	{
		if (droneState != DroneBehaviours::FASTATTACK)
		{
			seekScript->DisableMovement();
			patrolScript->StopPatrol();

			aiMovement->SetMovementStatuses(false, true);

			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			fastAttackScript->StartAttack();

			componentAnimation->SetParameter("IsSeeking", false);
			
			if (droneState == DroneBehaviours::SEEK)
			{
				componentAnimation->SetParameter("IsStopToAttack", true);
			}

			droneState = DroneBehaviours::FASTATTACK;
		}
	}
	else if (IsSpawnedEnemy() || ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(),
		seekDistance))
	{
		if (droneState == DroneBehaviours::PATROL)
		{
			patrolScript->StopPatrol();
			aiMovement->SetMovementStatuses(false, true);

			componentAnimation->SetParameter("IsSeeking", true);

			if (exclamationVFX)
			{
				exclamationVFX->Play();
			}

			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
			componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::ALERT);

			droneState = DroneBehaviours::ENEMY_DETECTED;
		}
		else if (droneState != DroneBehaviours::SEEK && droneState != DroneBehaviours::ENEMY_DETECTED)
		{
			minStopTimeAfterSeek -= deltaTime;

			if (minStopTimeAfterSeek <= 0.0f)
			{
				minStopTimeAfterSeek = minStopDurationAfterSeek;
				bool inFront = true;
				if (std::abs(ownerTransform->GetGlobalForward().
					AngleBetween(seekTargetTransform->GetGlobalPosition() - ownerTransform->GetGlobalPosition())) > 1.5708f)
				{
					inFront = false;
				}

				if (inFront || (ownerTransform->GetGlobalPosition().Equals(seekTargetTransform->GetGlobalPosition(),
					seekDistance / 2.0f) && !inFront)) //If is in front or if is not in front but close to the player
				{
					componentAnimation->SetParameter("IsSeeking", true);
					componentAnimation->SetParameter("IsAttacking", false);

					componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);

					if (droneState == DroneBehaviours::PATROL)//Play alert only when coming from patrol
					{
						componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::ALERT);
					}

					droneState = DroneBehaviours::SEEK;
				}
			}
		}
	}
	else if (droneState != DroneBehaviours::PATROL)
	{
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::PATROL);

		patrolScript->StartPatrol();

		componentAnimation->SetParameter("IsSeeking", false);
		componentAnimation->SetParameter("IsAttacking", false);

		droneState = DroneBehaviours::PATROL;

	}
}

void EnemyDroneScript::UpdateBehaviour(float deltaTime)
{
	float3 target = seekTargetTransform->GetGlobalPosition();

	switch (droneState)
	{
	case DroneBehaviours::PATROL:

		break;

	case DroneBehaviours::ENEMY_DETECTED:

		enemyDetectionTime += deltaTime;

		aiMovement->SetTargetPosition(target);
		aiMovement->SetRotationTargetPosition(target);

		if (enemyDetectionTime >= enemyDetectionDuration)
		{
			enemyDetectionTime = 0.0f;

			droneState = DroneBehaviours::SEEK;
		}

		break;

	case DroneBehaviours::SEEK:

		seekScript->Seeking();
		isFirstPatrolling = true;

		break;

	case DroneBehaviours::FASTATTACK:

		aiMovement->SetTargetPosition(target);
		aiMovement->SetRotationTargetPosition(target);

		if (componentAnimation->GetActualStateName() != "Flinch"
			&& componentAnimation->GetActualStateName() != "StopToAttack")
		{
			componentAnimation->SetParameter("IsStopToAttack", false);

			if (fastAttackScript->IsAttackAvailable())
			{
				fastAttackScript->PerformAttack();
				componentAnimation->SetParameter("IsAttacking", true);
			}

			if (fastAttackScript->NeedReposition())
			{
				CalculateNextPosition();
			}
		}
		else
		{
			if (fastAttackScript->IsPreShooting())
			{
				fastAttackScript->InterruptAttack();
			}
		}


		break;

	case DroneBehaviours::EXPLOSIONATTACK:

		aiMovement->SetTargetPosition(target);
		aiMovement->SetRotationTargetPosition(target);

		break;
	
	case DroneBehaviours::INPATH:

		if (pathScript->IsPathFinished())
		{
			droneState = DroneBehaviours::IDLE;
			componentAnimation->SetParameter("IsRunning", false);
			pathScript->Disable();
		}

		break;
	}
}

void EnemyDroneScript::ResetValues()
{
	componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_BEHAVIOURS);
	std::unordered_map<std::string, TypeFieldPairParameter> componentAnimationParameters =
		componentAnimation->GetStateMachine()->GetParameters();
	for (const std::pair<std::string, TypeFieldPairParameter>& parameter : componentAnimationParameters)
	{
		componentAnimation->SetParameter(parameter.first, false);
	}

	componentAnimation->SetParameter("IsSeeking", true);
	droneState = DroneBehaviours::INPATH;
	fastAttackScript->ResetScriptValues();
	healthScript->HealLife(1000.0f); // It will cap at max health
	EnemyDeathScript* enemyDeathScript = owner->GetComponent<EnemyDeathScript>();
	enemyDeathScript->ResetDespawnTimerAndEnableActions();
	if(pathScript)
	{
		pathScript->Enable();
		pathScript->ResetPath();
	}
}

void EnemyDroneScript::ParalyzeEnemy(bool nparalyzed)
{
	if (nparalyzed)
	{
		componentAnimation->SetParameter("IsRunning", false);
	}
	else
	{
		componentAnimation->SetParameter("IsRunning", true);
	}

	paralyzed = nparalyzed;
}

void EnemyDroneScript::CalculateNextPosition() const
{
	float3 nextPosition = ownerTransform->GetGlobalPosition() - seekTargetTransform->GetGlobalPosition();
	float distanceOS = nextPosition.Length();
	nextPosition.Normalize();

	float rotation = -(static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 0.5f)) + 1.0f);

	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if (r > 0.5)
	{
		rotation = std::abs(rotation);
	}

	float x = nextPosition.x;
	float z = nextPosition.z;
	nextPosition.x = x * Cos(rotation) - z * Sin(rotation);
	nextPosition.z = x * Sin(rotation) + z * Cos(rotation);
	nextPosition *= (attackDistance - 2);
	nextPosition += seekTargetTransform->GetGlobalPosition();
	nextPosition.y = seekTargetTransform->GetGlobalPosition().y;
	fastAttackScript->Reposition(nextPosition);
}

void EnemyDroneScript::SetReadyToDie()
{
	componentAnimation->SetParameter("IsTakingDamage", true);
	fastAttackScript->InterruptAttack();
	droneState = DroneBehaviours::READYTOEXPLODE;
}

void EnemyDroneScript::SetStunnedTime(float newTime)
{
	EnemyClass::SetStunnedTime(newTime);
	fastAttackScript->InterruptAttack();
}