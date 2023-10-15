#include "MeleeHeavyAttackBehaviourScript.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAgent.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/HealthSystem.h"
#include "../Scripts/EnemyDeathScript.h"

#include "../Scripts/SeekBehaviourScript.h"
#include "../Scripts/AIMovement.h"

#include "Auxiliar/Audio/AudioData.h"

#include "debugdraw.h"

REGISTERCLASS(MeleeHeavyAttackBehaviourScript);

MeleeHeavyAttackBehaviourScript::MeleeHeavyAttackBehaviourScript() : Script(), aiMovement(nullptr),
	attackState(ExplosionState::NOTDEAD), targetPlayer(nullptr), explosionDamage(30.0f), componentAnimation(nullptr),
	explosionTime(3.0f), parentDeathScript(nullptr), explosionDistance(4.0f), particleSystem(nullptr)
{
	REGISTER_FIELD(explosionDamage, float);
	REGISTER_FIELD(explosionTime, float);
	REGISTER_FIELD(explosionDistance, float);
	REGISTER_FIELD(particleSystem, ComponentParticleSystem*);
}

void MeleeHeavyAttackBehaviourScript::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	transform = owner->GetParent()->GetComponent<ComponentTransform>();
	parentDeathScript = owner->GetParent()->GetComponent<EnemyDeathScript>();
	componentAudioSource = owner->GetParent()->GetComponent<ComponentAudioSource>();
	ownerAgent = owner->GetParent()->GetComponentInternal<ComponentAgent>();
	componentAnimation = owner->GetParent()->GetComponent<ComponentAnimation>();
	aiMovement = owner->GetParent()->GetComponent<AIMovement>();
	rigidBody->SetKpForce(50);
}

void MeleeHeavyAttackBehaviourScript::Update(float deltaTime)
{
	if (isPaused) 
	{
		return;
	}

	rigidBody->SetPositionTarget(transform->GetGlobalPosition());

	if (attackState == ExplosionState::WAITING_EXPLOSION)
	{
		explosionTime -= deltaTime;
		UpdateDroneColor();

		if (explosionTime < 0)
		{
			attackState = ExplosionState::EXPLODING;
		}
	}

	else if (attackState == ExplosionState::EXPLODING)
	{
		if (particleSystem)
		{
			particleSystem->Stop();
		}

		if (targetPlayer && transform->GetGlobalPosition().Equals
		(targetPlayer->GetComponent<ComponentTransform>()->GetGlobalPosition(), explosionDistance))
		{
			targetPlayer->GetComponent<HealthSystem>()->TakeDamage(explosionDamage);
		}

		if (ownerAgent)
		{
			ownerAgent->SetMaxAcceleration(ownerAgent->GetMaxAcceleration() / 2.0f);
		}
		else
		{
			//set new target position
			owner->GetParent()->GetComponent<ComponentRigidBody>()->SetKpForce(0.5f);
		}
		
		componentAnimation->SetParameter("IsDead", true);
		attackState = ExplosionState::DEAD;
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::STOP_TIMER);
		componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::EXPLOSION);
		parentDeathScript->ManageEnemyDeath();
	}
}

void MeleeHeavyAttackBehaviourScript::UpdateDroneColor()
{
	//When we have tint the color of the drone will change.
}

void MeleeHeavyAttackBehaviourScript::TriggerExplosion()
{
	SeekBehaviourScript* enemySeekBehaviour = owner->GetParent()->GetComponent<SeekBehaviourScript>();
	float3 targetPos = enemySeekBehaviour->GetTarget()->GetComponent<ComponentTransform>()->GetGlobalPosition();

	if (particleSystem)
	{
		particleSystem->Play();
	}
	
	componentAudioSource->PostEvent(AUDIO::SFX::NPC::DRON::TIMER);
	attackState = ExplosionState::WAITING_EXPLOSION;
}

ExplosionState MeleeHeavyAttackBehaviourScript::HasExploded() const
{
	return attackState;
}

void MeleeHeavyAttackBehaviourScript::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		targetPlayer = other->GetOwner();
	}
}

void MeleeHeavyAttackBehaviourScript::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		targetPlayer = nullptr;
	}
}

void MeleeHeavyAttackBehaviourScript::SetIsPaused(bool isPaused)
{
	this->isPaused = isPaused;
}
