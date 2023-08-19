#include "StdAfx.h"
#include "FinalBossScript.h"

#include "Components/ComponentScript.h"

#include "../Scripts/PatrolBehaviourScript.h"

REGISTERCLASS(FinalBossScript);

FinalBossScript::FinalBossScript() : bossState(FinalBossStates::NEUTRAL), patrolScript(nullptr)
{
}

void FinalBossScript::Start()
{
	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
}

void FinalBossScript::Update(float deltaTime)
{
	if (bossState == FinalBossStates::NEUTRAL)
	{
		patrolScript->Patrolling();
	}
}