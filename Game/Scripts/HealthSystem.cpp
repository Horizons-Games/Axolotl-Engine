#include "HealthSystem.h"

#include "AxoLog.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentParticleSystem.h"

#include "../Scripts/BixAttackScript.h"
#include "../Scripts/PlayerDeathScript.h"
#include "../Scripts/EnemyDeathScript.h"
#include "../Scripts/PlayerManagerScript.h"
#include "MeshEffect.h"

REGISTERCLASS(HealthSystem);

#define TIME_BETWEEN_EFFECTS 0.05f
#define MAX_TIME_EFFECT_DURATION 0.1f

HealthSystem::HealthSystem() : Script(), currentHealth(100), maxHealth(100), componentAnimation(nullptr), 
isImmortal(false), enemyParticleSystem(nullptr), attackScript(nullptr), hitEffectDuration(0.f), hasTakenDamage(false)
{
	REGISTER_FIELD(currentHealth, float);
	REGISTER_FIELD(maxHealth, float);
	REGISTER_FIELD(isImmortal, bool);
	REGISTER_FIELD(enemyParticleSystem, GameObject*);
}

void HealthSystem::Start()
{
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	//componentParticleSystem = enemyParticleSystem->GetComponent<ComponentParticleSystem>();

	//--- This was done because in the gameplay scene there is no particle system
	try
	{
		componentParticleSystem = owner->GetComponent<ComponentParticleSystem>();
	}

	catch (const ComponentNotFoundException&)
	{
		componentParticleSystem = nullptr;
	}
	//---

	// Check that the currentHealth is always less or equal to maxHealth
	if (maxHealth < currentHealth)
	{
		maxHealth = currentHealth;
	}

	meshEffect = owner->GetComponent<MeshEffect>();
	meshEffect->ReserveSpace(1);
	meshEffect->AddColor(float3(1.f, 0.f, 0.f));

	if (owner->CompareTag("Player"))
	{
		attackScript = owner->GetComponent<BixAttackScript>();
	}
}

void HealthSystem::Update(float deltaTime)
{
	meshEffect->DamageEffect();

	if (!EntityIsAlive() && owner->CompareTag("Player"))
	{
		meshEffect->ClearEffect();
		PlayerDeathScript* playerDeathManager = owner->GetComponent<PlayerDeathScript>();
		playerDeathManager->ManagePlayerDeath();
	}

	else if (!EntityIsAlive() && owner->CompareTag("Enemy"))
	{
		meshEffect->ClearEffect();
		EnemyDeathScript* enemyDeathManager = owner->GetComponent<EnemyDeathScript>();
		enemyDeathManager->ManageEnemyDeath();
	}

	// This if/else should ideally be called inside the TakeDamage function
	// 
	// By setting this here, we make certain that 'IsTakingDamage' remains as true during a couple frames
	// so the state machine could behave correctly (we could delete this once we have a way to delay any function calls)
	if (currentHealth <= 0)
	{
		componentAnimation->SetParameter("IsDead", true);
	}

	else
	{
		componentAnimation->SetParameter("IsTakingDamage", false);
	}
}

void HealthSystem::TakeDamage(float damage)
{
	if (!isImmortal) 
	{
		if (owner->CompareTag("Player") && !attackScript->IsPerfomingJumpAttack())
		{
			float playerDefense = owner->GetComponent<PlayerManagerScript>()->GetPlayerDefense();
			float actualDamage = std::max(damage - playerDefense, 0.f);

			currentHealth -= actualDamage;
		}

		else
		{
			currentHealth -= damage;
		}

		componentAnimation->SetParameter("IsTakingDamage", true);
		
		if (EntityIsAlive())
		{
			meshEffect->StartEffect(MAX_TIME_EFFECT_DURATION, TIME_BETWEEN_EFFECTS);
		}

		if (componentParticleSystem)
		{
			componentParticleSystem->Play();
		}

		// componentParticleSystem->Pause();
	}
}

void HealthSystem::HealLife(float amountHealed)
{
	currentHealth = std::min(currentHealth + amountHealed, maxHealth);
}

bool HealthSystem::EntityIsAlive() const
{
	return currentHealth > 0.f;
}

float HealthSystem::GetMaxHealth() const
{
	return maxHealth;
}

bool HealthSystem::IsImmortal() const
{
	return isImmortal;
}

void HealthSystem::SetIsImmortal(bool isImmortal)
{
	this->isImmortal = isImmortal;
}

float HealthSystem::GetCurrentHealth() const
{
	return currentHealth;
}
