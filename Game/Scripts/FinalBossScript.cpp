#include "StdAfx.h"
#include "FinalBossScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentRigidBody.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(FinalBossScript);

FinalBossScript::FinalBossScript() : bossState(FinalBossStates::NEUTRAL), patrolScript(nullptr), 
	bossHealthSystem(nullptr), rigidBody(nullptr)
{
}

void FinalBossScript::Start()
{
	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	bossHealthSystem = owner->GetComponent<HealthSystem>();

	rigidBody = owner->GetComponent<ComponentRigidBody>();
	rigidBody->SetKpForce(0.25f);
}

void FinalBossScript::Update(float deltaTime)
{
	ManageChangePhase();

	if (bossState == FinalBossStates::NEUTRAL)
	{
		patrolScript->Patrolling();
	}
}

void FinalBossScript::ManageChangePhase()
{
	if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.2f)
	{
		bossState = FinalBossStates::LAST_RESORT;

		LOG_VERBOSE("Final Boss is in LAST RESORT");
	}

	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.5f)
	{
		bossState = FinalBossStates::DEFENSIVE;

		LOG_VERBOSE("Final Boss is DEFENSIVE");
	}

	else if (bossHealthSystem->GetCurrentHealth() < bossHealthSystem->GetMaxHealth() * 0.8f)
	{
		bossState = FinalBossStates::AGGRESSIVE;

		LOG_VERBOSE("Final Boss is AGGRESSIVE");
	}

	else
	{
		bossState = FinalBossStates::NEUTRAL;

		LOG_VERBOSE("Final Boss is NEUTRAL");
	}
}